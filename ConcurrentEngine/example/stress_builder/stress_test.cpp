#if 0    // stress_test on FIFOScheduler & PriorityScheduler
#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <threadPool/threadPool.hpp>
#include <threadPool/scheduler/PriorityScheduler.hpp>
#include <threadPool/scheduler/DAGschedule.hpp>
#include <threadPool/scheduler/FIFO_schedule.hpp>
#include <iostream>

using namespace std;

string random_string(size_t length) 
{
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static thread_local mt19937 rg{ random_device{}() };
    static thread_local uniform_int_distribution<> pick(0, sizeof(charset) - 2);

    string s;
    s.reserve(length);
    while (length--)
        s += charset[pick(rg)];
    return s;
}

void stress_test(ConcurrentEngine::ThreadPool& pool, std::string schedulerName, int num_tasks) 
{
    auto start = chrono::high_resolution_clock::now();

    vector<future<void>> results;
    results.reserve(num_tasks);

    for (int i = 0; i < num_tasks; i++) 
    {
        results.push_back(pool.submit("redis_task", [i]() -> void {   
            string key = "key_" + to_string(i % 1000);  
            string value = random_string(16);

            // if (i < 5) {
            //     if (i % 3 == 0) {
            //         std::cout << "[Task " << i << "] SET " << key << " = " << value << std::endl;
            //     } else if (i % 3 == 1) {
            //         std::cout << "[Task " << i << "] GET " << key << std::endl;
            //     } else {
            //         std::cout << "[Task " << i << "] DEL " << key << std::endl;
            //     }
            // }

            this_thread::sleep_for(1ms);
        }));
    }

    for (auto& r : results) 
        r.get();
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    std::cout << "[StressTest] " << num_tasks 
              << " tasks on " << schedulerName
              << " finished in " << duration << " ms" << std::endl;
}


int main() 
{
    ConcurrentEngine::ThreadPool pool;
    ThreadLogger::getInstance().enableConsoleLogging(false);

    pool.setMode(ConcurrentEngine::PoolMode::MODE_CACHED);
    pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
    pool.start(8);
    stress_test(pool, "FIFOScheduler", 100000);
    pool.stop();

    pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::PriorityScheduler>());
    pool.start(8);
    stress_test(pool, "PriorityScheduler", 100000);
    pool.stop();


    return 0;
}




#endif

#if 0    // auto stress_test 
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <threadPool/threadPool.hpp>
#include <threadPool/scheduler/FIFO_schedule.hpp>
#include <threadPool/scheduler/PriorityScheduler.hpp>
#include <iomanip>

using namespace std;

// 模擬壓力任務
void stress_test_auto(ConcurrentEngine::ThreadPool& pool, int num_tasks) 
{
    vector<future<void>> results;
    results.reserve(num_tasks);
    for (int i = 0; i < num_tasks; i++) 
    {
        results.push_back(pool.submit("redis_task", [i]() {
            this_thread::sleep_for(1ms);
        }));
    }

    for (auto& r : results) 
        r.get();
}

int main() 
{
    // ThreadPool 初始化
    ConcurrentEngine::ThreadPool pool;
    ThreadLogger::getInstance().enableConsoleLogging(false);
    pool.setMode(ConcurrentEngine::PoolMode::MODE_CACHED);

    // 任務數量列表
    vector<int> task_sizes = {1000, 10000, 50000, 100000, 200000, 500000};

    // CSV 使用 RAII，確保關閉 flush
    ofstream csvFile("stress_report.csv");
    if (!csvFile.is_open()) {
        cerr << "[Error] Failed to open stress_report.csv for writing.\n";
        return 1;
    }
    csvFile << "Scheduler,Tasks,Time_ms\n";

    for (auto scheduler_type : {"FIFO", "PRIORITY"}) {
        cout << "\n=== Testing " << scheduler_type << " Scheduler ===\n";
        cout << setw(12) << "Tasks" 
             << setw(12) << "Time(ms)" << endl;

        for (auto num_tasks : task_sizes) {
            try {
                // 設定 Scheduler
                pool.stop();
                pool.start(8);
                if (string(scheduler_type) == "FIFO")
                    pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
                else
                    pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::PriorityScheduler>());

                auto start = chrono::high_resolution_clock::now();

                stress_test_auto(pool, num_tasks);

                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                cout << setw(12) << num_tasks 
                     << setw(12) << duration << endl;

                // 寫入 CSV
                csvFile << scheduler_type << "," << num_tasks << "," << duration << "\n";
                csvFile.flush(); // 立即 flush

            } catch (const std::exception& e) {
                cout << "[StressTest] " << num_tasks 
                     << " tasks FAILED: " << e.what() << endl;
                break;
            }
        }
    }

    cout << "\n[StressTest] Report saved to stress_report.csv\n";
    return 0;
}

#endif

#if 0 // auto stress_test v2

#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>
#include <future>
#include <atomic>
#include <iostream>

#include <threadPool/threadPool.hpp>
#include <threadPool/scheduler/FIFO_schedule.hpp>
#include <threadPool/scheduler/PriorityScheduler.hpp>

using namespace std;

struct StressResult {
    int tasks;
    long long submit_ms;
    long long exec_ms;
    long long total_ms;
    int completed;
};

StressResult stress_test_auto(ConcurrentEngine::ThreadPool& pool, int num_tasks) 
{
    atomic<int> counter = 0;
    vector<future<void>> results;
    results.reserve(num_tasks);

    auto start_submit = chrono::high_resolution_clock::now();
    for (int i = 0; i < num_tasks; i++) {
        results.push_back(pool.submit("redis_task", [&counter]() {
            this_thread::sleep_for(1ms);
            counter.fetch_add(1, memory_order_relaxed);
        }));
    }
    auto end_submit = chrono::high_resolution_clock::now();

    for (auto& r : results) r.get();
    auto end_all = chrono::high_resolution_clock::now();

    auto submit_ms = chrono::duration_cast<chrono::milliseconds>(end_submit - start_submit).count();
    auto exec_ms   = chrono::duration_cast<chrono::milliseconds>(end_all - end_submit).count();
    auto total_ms  = chrono::duration_cast<chrono::milliseconds>(end_all - start_submit).count();

    return { num_tasks, submit_ms, exec_ms, total_ms, counter.load() };
}

int main() 
{
    ConcurrentEngine::ThreadPool pool;
    ThreadLogger::getInstance().enableConsoleLogging(false);
    pool.setMode(ConcurrentEngine::PoolMode::MODE_CACHED);
//  , 10000, 50000, 100000, 200000, 500000
    vector<int> task_sizes = {1000};

    ofstream csvFile("stress_report.csv");
    if (!csvFile.is_open()) {
        cerr << "[Error] Failed to open stress_report.csv for writing.\n";
        return 1;
    }
    csvFile << "Scheduler,Tasks,Submit_ms,Exec_ms,Total_ms,Completed\n";

    for (auto scheduler_type : {"FIFO", "PRIORITY"}) {
        cout << "\n=== Testing " << scheduler_type << " Scheduler ===\n";
        cout << setw(12) << "Tasks"
             << setw(12) << "Submit"
             << setw(12) << "Exec"
             << setw(12) << "Total"
             << setw(12) << "Completed" << endl;

        for (auto num_tasks : task_sizes) {
            try {
                pool.stop();
                pool.start(8);

                if (string(scheduler_type) == "FIFO")
                    pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
                else
                    pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::PriorityScheduler>());

                auto result = stress_test_auto(pool, num_tasks);

                cout << setw(12) << result.tasks
                     << setw(12) << result.submit_ms
                     << setw(12) << result.exec_ms
                     << setw(12) << result.total_ms
                     << setw(12) << result.completed << endl;

                csvFile << scheduler_type << ","
                        << result.tasks << ","
                        << result.submit_ms << ","
                        << result.exec_ms << ","
                        << result.total_ms << ","
                        << result.completed << "\n";
                csvFile.flush();

                if (result.completed != result.tasks) {
                    cerr << "[Warning] " << scheduler_type << " "
                         << result.tasks << " tasks, only "
                         << result.completed << " completed!\n";
                }

            } catch (const exception& e) {
                cerr << "[StressTest] " << num_tasks
                     << " tasks FAILED: " << e.what() << endl;
                break;
            }
        }
    }

    cout << "\n[StressTest] Report saved to stress_report.csv\n";
    return 0;
}

#endif 

#if 1 // auto stress_test v3
// goal: each scheduler use ThreadPool independently
//       after CSV output use PowerShell to run Python analysis script
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>
#include <future>
#include <atomic>
#include <iostream>
#include <cstdlib>   // for system()
#include <exception>

#include <threadPool/threadPool.hpp>
#include <threadPool/scheduler/FIFO_schedule.hpp>
#include <threadPool/scheduler/PriorityScheduler.hpp>

using namespace std;
struct StressResult 
{
    int tasks;
    long long submit_ms;
    long long exec_ms;
    long long total_ms;
    int completed;
};

StressResult stress_test_auto(ConcurrentEngine::ThreadPool& pool, int num_tasks) 
{
    std::atomic<int> counter{0};
    std::vector<std::future<void>> results;
    results.reserve(num_tasks);

    auto start_submit = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_tasks; ++i) 
    {
        results.push_back(pool.submit("redis_task", [&counter]() {
            try {
                std::this_thread::sleep_for(1ms);// working 1ms each task
                counter.fetch_add(1, std::memory_order_relaxed);
            } catch (...) {
                // task internal exception will be thrown in future.get()
                throw;
            }
        }));
    }

    auto end_submit = std::chrono::high_resolution_clock::now();

    // wait for futures to complete
    for (auto &f : results) 
    {
        if (!f.valid()) continue;
        try {
            f.get(); // if task throws exception, it will be caught here
        } catch (const std::exception &e) {
            std::cerr << "[Task Exception] " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[Task Exception] unknown exception in task" << std::endl;
        }
    }

    auto end_all = std::chrono::high_resolution_clock::now();

    long long submit_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_submit - start_submit).count();
    long long exec_ms   = std::chrono::duration_cast<std::chrono::milliseconds>(end_all - end_submit).count();
    long long total_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(end_all - start_submit).count();

    return { num_tasks, submit_ms, exec_ms, total_ms, counter.load() };
}


int main()
{
    try {
        std::vector<int> task_sizes;
        for (int n = 2500; n <= 100000; n += 2500) 
            task_sizes.push_back(n);

        ofstream csvFile("stress_report.csv");

        if (!csvFile.is_open()) 
        {
            cerr << "[Error] Failed to open stress_report.csv for writing.\n";
            return 1;
        }
        csvFile << "Scheduler,Tasks,Submit_ms,Exec_ms,Total_ms,Completed\n";

        for (auto scheduler_type : {"FIFO", "PRIORITY"}) 
        {
            cout << "\n=== Testing " << scheduler_type << " Scheduler ===\n";
            cout << setw(12) << "Tasks"
                 << setw(12) << "Submit"
                 << setw(12) << "Exec"
                 << setw(12) << "Total"
                 << setw(12) << "Completed" << endl;

            for (auto num_tasks : task_sizes) 
            {
                try {
                    // new ThreadPool for each test
                    ConcurrentEngine::ThreadPool pool;
                    ThreadLogger::getInstance().enableConsoleLogging(false);
                    pool.setMode(ConcurrentEngine::PoolMode::MODE_CACHED);
                    
                    if (string(scheduler_type) == "FIFO") 
                        pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
                    else 
                        pool.setScheduler(make_unique<ConcurrentEngine::Scheduler::PriorityScheduler>());

                    pool.start(512);
                    auto result = stress_test_auto(pool, num_tasks);
                    pool.stop();// stop pool after all futures have completed

                    cout << setw(12) << result.tasks
                         << setw(12) << result.submit_ms
                         << setw(12) << result.exec_ms
                         << setw(12) << result.total_ms
                         << setw(12) << result.completed << endl;

                    csvFile << scheduler_type << ","
                            << result.tasks << ","
                            << result.submit_ms << ","
                            << result.exec_ms << ","
                            << result.total_ms << ","
                            << result.completed << "\n";
                    csvFile.flush();

                    if (result.completed != result.tasks) 
                    {
                        cerr << "[Warning] " << scheduler_type << " "
                             << result.tasks << " tasks, only "
                             << result.completed << " completed!\n";
                    }

                } catch (const std::exception& e) {
                    cerr << "[StressTest] " << num_tasks
                         << " tasks FAILED: " << e.what() << endl;
                } catch (...) {
                    cerr << "[StressTest] " << num_tasks
                         << " tasks FAILED: unknown exception\n";
                }
            }
        }
        
    
        csvFile.close();
        cout << "\n[StressTest] Report saved to stress_report.csv\n";

        // 使用 PowerShell 去執行 Python 分析腳本（在同一個目錄下）
        // -NoProfile: 不載入使用者設定
        // -ExecutionPolicy Bypass: 允許臨時執行腳本（避免執行政策阻擋）
        // 注意：這裡直接呼叫 python（假設 python 可透過 PATH 呼叫）
        string psCmd = "powershell -NoProfile -ExecutionPolicy Bypass -Command \"python stress_analyze.py\"";
        cout << "[Run] Invoking Python analysis via PowerShell...\n";
        int ret = system(psCmd.c_str());
        if (ret != 0) 
        {
            cerr << "[Error] Python analysis failed (return code = " << ret << ")\n";
            return 2;
        }

        cout << "[Done] All results generated successfully.\n";
        return 0;

    } catch (const std::exception &e) {
        cerr << "[Fatal] Exception: " << e.what() << endl;
        return 3;
    } catch (...) {
        cerr << "[Fatal] Unknown exception\n";
        return 4;
    }
}

#endif

