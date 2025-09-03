#ifndef REDISSERVER_HPP
#define REDISSERVER_HPP

#include <future>
#include <threadPool/threadPool.hpp> 
#include "RedisDatabase.hpp"
#include "CommandParser.hpp"

class RedisServer 
{
public:
    RedisServer(ConcurrentEngine::ThreadPool& pool);

    // FIFO / PriorityScheduler
    std::future<std::string> submitCommand(const Command& cmd);
    
    // 專用 DAG 提交：返回 TaskNode，不會在 task 內 get()
    std::shared_ptr<ConcurrentEngine::Scheduler::TaskNode>
    submitCommandDAG(const Command& cmd,
                     const std::vector<std::shared_ptr<ConcurrentEngine::Scheduler::TaskNode>>& deps = {})
    {
        auto promisePtr = std::make_shared<std::promise<std::string>>();
        auto fut = promisePtr->get_future();

        auto taskNode = std::make_shared<ConcurrentEngine::Scheduler::TaskNode>(
            [this, cmd, promisePtr]() {
                try {
                    std::string result;
                    switch(cmd.type)
                    {
                        case CommandType::SET: db_.set(cmd.key, cmd.value); result = "OK"; break;
                        case CommandType::GET: result = db_.get(cmd.key); break;
                        // 其他指令...
                        default: result = "ERR unknown command"; break;
                    }
                    promisePtr->set_value(result);
                } catch (...) {
                    promisePtr->set_exception(std::current_exception());
                }
            }
        );

        pool_.submitDAG(taskNode, deps);
        // 將 future 存起來，外面再 get()
        nodeFutures_[taskNode] = std::move(fut);
        return taskNode;
    }

    // 外部拿結果用
    std::shared_future<std::string> getResult(std::shared_ptr<ConcurrentEngine::Scheduler::TaskNode> node) 
    {    return nodeFutures_.at(node);    }

private:
    std::unordered_map<std::shared_ptr<ConcurrentEngine::Scheduler::TaskNode>,
                       std::shared_future<std::string>> nodeFutures_;
private:
    RedisDatabase db_;
    ConcurrentEngine::ThreadPool& pool_;
};

#endif // REDISSERVER_HPP

