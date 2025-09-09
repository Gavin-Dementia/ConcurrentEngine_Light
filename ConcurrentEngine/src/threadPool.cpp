// namespace ConcurrentEngine
#include <threadPool/threadPool.hpp>
#include <algorithm>

namespace ConcurrentEngine 
{

// 啟動 ThreadPool，建立指定數量的工作執行緒
void ThreadPool::start(int threadCount)
{
    if (state_ && state_->isRunning) return;

    state_ = std::make_shared<ThreadPoolState>();
    ThreadLogger::getInstance().log("[ThreadPool] Starting with requested " + std::to_string(threadCount) + " threads.");

    switch (state_->poolmode)
    {
        case PoolMode::MODE_SINGLE:
            threadCount = 1;
            state_->maxThreadCount = 1;
            break;

        case PoolMode::MODE_FIXED:
            state_->maxThreadCount = threadCount;
            break;

        case PoolMode::MODE_CACHED:
        {
            size_t hw = std::thread::hardware_concurrency();
            if (hw == 0) hw = 2; // fallback: 無法偵測核心數時至少 2

            threadCount = std::max<size_t>(2, hw);      // 初始線程數量
            state_->maxThreadCount = std::max<size_t>(threadCount, 32); // 最大上限，可依需求調整
            break;
        }

        default:
            break;
    }

    ThreadLogger::getInstance().log("[ThreadPool] PoolMode: " + std::to_string(static_cast<int>(state_->poolmode)) +
                                   ", Initial threads: " + std::to_string(threadCount) +
                                   ", Max threads: " + std::to_string(state_->maxThreadCount));

    for (int i = 0; i < threadCount; ++i)
    {
        int threadId = state_->threadIDCounter++;
        auto meta = std::make_shared<ThreadMeta>(threadId);
        {
            std::lock_guard<std::mutex> lock(state_->threadMapMutex);
            threadMetas_[threadId] = meta;
        }

        workers_.emplace_back(&ThreadPool::workerThreadFunc, this, threadId);
        state_->curThreadCount++;
    }

    state_->isRunning = true;
    ThreadLogger::getInstance().log("[ThreadPool] State set to running.");

}

// 停止 ThreadPool，通知所有工作執行緒結束並等待它們 join
void ThreadPool::stop()
{
    if (!state_->isRunning) return;

    state_->isRunning = false;

    scheduler_->notifyAll(); // 通知 Scheduler 停止，喚醒所有阻塞執行緒
    ThreadLogger::getInstance().log("[ThreadPool] Stopping...");

    for (auto& t : workers_)
    {
        if (t.joinable())
            t.join();
    }

    ThreadLogger::getInstance().log("[ThreadPool] All worker threads joined.");
}

// 依 threadId 取得 ThreadMeta（紀錄該執行緒狀態）
std::shared_ptr<ThreadMeta> ThreadPool::getThreadMeta(int tid)
{
    std::lock_guard<std::mutex> lock(state_->threadMapMutex);
    auto it = threadMetas_.find(tid);
    if (it != threadMetas_.end())
            return it->second;
    else
        return nullptr;
}

// 支援以 threadId 追蹤執行緒狀態的工作函式
void ThreadPool::workerThreadFunc(int threadId)
{
    auto meta = getThreadMeta(threadId);
    if (!meta) 
    {
        ThreadLogger::getInstance().log("[Worker] Error: No ThreadMeta found", LogLevel::ERROR, threadId);
        return;
    }

    ThreadLogger::getInstance().log("[Worker] Thread started", LogLevel::INFO, threadId);

    while (state_->isRunning)
    {
        Scheduler::Task task;

        if (state_->poolmode == PoolMode::MODE_CACHED)
        {
            using namespace std::chrono_literals;
            // 嘗試取得任務，最多等待 60 秒
            task = scheduler_->getTaskFor(60s);

            if (!task) 
            {
                // 超時沒有任務 → 自動回收 worker
                ThreadLogger::getInstance().log("[Worker] Idle timeout, exiting (cached mode)", LogLevel::INFO, threadId);
                {
                    std::lock_guard<std::mutex> lock(state_->threadMapMutex);
                    threadMetas_.erase(threadId);
                    state_->curThreadCount--;
                }
                meta->markTerminating();
                meta->markTerminated();
                return;
            }
        }
        else // Fixed / Single 模式：永遠阻塞直到拿到任務
           task = scheduler_->getTask();
        

        // 如果拿不到任務且 ThreadPool 停止，直接退出
        if (!task && !state_->isRunning) break;

        meta->markRunning();
        ThreadLogger::getInstance().log("[Worker] Task started", LogLevel::DEBUG, threadId);

        try 
        {  if (task) task();  } 
        catch (const std::exception& e) 
        {
            ThreadLogger::getInstance().log(std::string("[Worker] Task exception: ") + e.what(), LogLevel::ERROR, threadId);
        }
        catch (...)
        {
            ThreadLogger::getInstance().log("[Worker] Unknown exception occurred in task", LogLevel::ERROR, threadId);
        }

        ThreadLogger::getInstance().log("[Worker] Task finished", LogLevel::DEBUG, threadId);
        meta->markIdle();
    }

    // ThreadPool 停止時的清理
    meta->markTerminating();
    ThreadLogger::getInstance().log("[Worker] Thread exiting", LogLevel::INFO, threadId);
    meta->markTerminated();
}

// 提交普通任務，帶優先級的版本
bool ThreadPool::submit(Scheduler::Task task, Scheduler::TaskPriority priority)
{
    if (!scheduler_) 
    {
        ThreadLogger::getInstance().log("[ThreadPool] Submit failed: No scheduler.", LogLevel::ERROR);
        return false;
    }

    if (!state_ || !state_->isRunning)
    {
        ThreadLogger::getInstance().log("[ThreadPool] Submit failed: Not running.", LogLevel::ERROR);
        return false;
    }

    ThreadLogger::getInstance().log("[ThreadPool] Task submitted with priority " 
                                    + std::to_string(static_cast<int>(priority)), LogLevel::INFO);

    // === Cached 模式：如果沒有空閒執行緒，且還沒達到上限 -> 動態新增 ===
    if (state_->poolmode == PoolMode::MODE_CACHED)
    {
        if (state_->freeThread == 0 && state_->curThreadCount < state_->maxThreadCount)
        {
            int tid = state_->threadIDCounter++;
            auto meta = std::make_shared<ThreadMeta>(tid);
            {
                std::lock_guard<std::mutex> lock(state_->threadMapMutex);
                threadMetas_[tid] = meta;
            }
            workers_.emplace_back(&ThreadPool::workerThreadFunc, this, tid);
            state_->curThreadCount++;

            ThreadLogger::getInstance().log("[ThreadPool] Cached mode: Spawned new worker " + std::to_string(tid), LogLevel::INFO);
        }
    }

    // === DAG 調度器不接受普通任務直接提交 ===
    if (dynamic_cast<Scheduler::DAGScheduler*>(scheduler_.get())) 
    {
        ThreadLogger::getInstance().log("[ThreadPool] DAG Scheduler does not accept plain Task submit.", LogLevel::WARN);
        return false;
    }

    // === Priority 調度器 ===
    if (auto* pri = dynamic_cast<Scheduler::PriorityScheduler*>(scheduler_.get()))
    {
        pri->addTask(std::move(task), priority);
    }
    else
    {
        // === FIFO / 其他 ===
        scheduler_->addTask(std::move(task));
    }

    return true;
}

// 專用 DAG 任務提交（包含依賴）
bool ThreadPool::submitDAG(std::shared_ptr<Scheduler::TaskNode> node,
                   const std::vector<std::shared_ptr<Scheduler::TaskNode>>& deps)
{
    if (!scheduler_ || !state_ || !state_->isRunning) return false;

    auto* dag = dynamic_cast<Scheduler::DAGScheduler*>(scheduler_.get());
    LOG_INFO("==== submitDAG ====");

    if (!dag) 
    {
        LOG_ERROR("[ThreadPool] Current scheduler is not DAG.");
        return false;
    }

    if (!node || !node->task) 
    {
        LOG_ERROR("[ThreadPool] ERROR: submitDAG received invalid task.");
        return false;
    }

    dag->addTask(node, deps);
    ThreadLogger::getInstance().log("[ThreadPool] DAG task submitted.");
    return true;
}

} // namespace ConcurrentEngine

