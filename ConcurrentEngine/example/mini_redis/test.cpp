#if 0
#include <iostream>
#include "RedisDatabase.hpp"

int main() 
{
    RedisDatabase db;

    db.set("name", "Gavin");
    auto value = db.get("name");
    if (value) 
        std::cout << "GET name: " << *value << std::endl;
    
    db.del("name");
    if (!db.get("name")) 
        std::cout << "name had been deleted" << std::endl;
    
    return 0;
}
#endif

#if 0
#include "CommandParser.hpp"
#include <iostream>

int main() 
{
    CommandParser parser;

    std::string input[] = {
        "SET name Gavin",
        "GET name",
        "DEL name",
        "INVALIDCOMMAND",
        "GET non_existing_key",
        "SET key1 value1",
        "EXISTS key1",
        "PING",
        "QUIT",
        "SET name",     // invalid
        "GET"           // invalid
    };

    for (const auto& line : input) 
    {
        Command cmd = parser.parse(line);
        std::cout << "Input: " << line << "\n";

        switch(cmd.type)
        {
            case CommandType::GET: std::cout << "Parsed: GET " << cmd.key << "\n"; break;
            case CommandType::SET: std::cout << "Parsed: SET " << cmd.key << " = " << cmd.value << "\n"; break;
            case CommandType::DEL: std::cout << "Parsed: DEL " << cmd.key << "\n"; break;
            case CommandType::EXISTS: std::cout << "Parsed: EXISTS " << cmd.key << "\n"; break;
            case CommandType::PING: std::cout << "Parsed: PING\n"; break;
            case CommandType::QUIT: std::cout << "Parsed: QUIT\n"; break;
            case CommandType::INVALID: std::cout << "Parsed: INVALID COMMAND\n"; break;
        }

        std::cout << "-------------------\n";
    }

    return 0;
}
#endif

#if 0

#include "CommandParser.hpp"
#include "RedisDatabase.hpp"
#include <iostream>

int main() 
{
    RedisDatabase db;
    CommandParser parser;

    std::string line;
    while (std::getline(std::cin, line)) 
    {
        Command cmd = parser.parse(line);

        switch (cmd.type) 
        {
            case CommandType::SET:
                db.set(cmd.key, cmd.value);
                std::cout << "OK\n";
                break;

            case CommandType::GET: 
            {
                auto val = db.get(cmd.key);
                if (val) std::cout << *val << "\n";
                else std::cout << "(nil)\n";
                break;
            }

            case CommandType::DEL:
                db.del(cmd.key);
                std::cout << "OK\n";
                break;

            case CommandType::INVALID:
                std::cout << "Error: invalid command\n";
                break;

            case CommandType::QUIT:
                std::cout << "Bye!\n";
                return 0;
            
            case CommandType::HELP:
                std::cout << R"(
                Available commands:
                SET "key" "value"       - Set a value
                GET "key"             - Get a value
                DEL "key"             - Delete a key
                EXISTS "key"          - Check if key exists
                PING                - Check server status (returns PONG)
                QUIT                - Exit the program
                HELP                - Show this help message
                )" << std::endl;
                break;
            
            case CommandType::EXISTS:
                if (db.get(cmd.key)) 
                    std::cout << "Key exists\n";
                else 
                    std::cout << "Key does not exist\n";
                break;

            // case CommandType::PING:

            default:
                std::cout << "Unsupported command.\n";
                break;
        }
    }

    return 0;
}

#endif

#if 0

#include <iostream>
#include <cassert>
#include "CommandExecutor.hpp"
#include "RedisDatabase.hpp"

void test_set_and_get() 
{
    RedisDatabase db;
    CommandExecutor executor(db);

    std::string result1 = executor.execute( "SET", "foo", "bar");
    assert(result1 == "OK");

    std::string result2 = executor.execute("GET", "foo");
    assert(result2 == "bar");

    std::string result3 = executor.execute("GET", "unknown");
    assert(result3 == "(nil)");

    std::cout << "test_set_and_get passed!" << std::endl;
}

void test_del() 
{
    RedisDatabase db;
    CommandExecutor executor(db);

    executor.execute("SET", "key1", "value1");
    executor.execute("SET", "key2", "value2");

    std::string result1 = executor.execute("DEL", "key1");
    assert(result1 == "1");  // one key deleted

    std::string result2 = executor.execute("DEL", "key1");
    assert(result2 == "0");  // key does not exist anymore

    std::cout << "test_del passed!" << std::endl;
}

void test_help() 
{
    RedisDatabase db;
    CommandExecutor executor(db);

    std::string result = executor.execute("HELP");
    assert(result.find("SET") != std::string::npos);
    assert(result.find("GET") != std::string::npos);

    std::cout << "test_help passed!" << std::endl;
}

int main() 
{
    test_set_and_get();
    test_del();
    test_help();

    std::cout << "All CommandExecutor tests passed!" << std::endl;
    return 0;
}


#endif


#if 0

#include "RedisServer.hpp"
#include "RedisDatabase.hpp"
#include "CommandParser.hpp"
#include "CommandExecutor.hpp"

#include <sstream>
#include <iostream>
#include <cassert>

void test_redis_server()
{
    // Simulate user input
    std::istringstream input(
        "SET name Gavin\n"
        "GET name\n"
        "DEL name\n"
        "GET name\n"
        "QUIT\n"
    );

    // Capture output
    std::ostringstream output;

    // Redirect std::cin and std::cout temporarily
    auto cin_buf = std::cin.rdbuf();
    auto cout_buf = std::cout.rdbuf();
    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());

    // Run server
    RedisServerConcurrent server;
    server.run();

    // Restore std::cin and std::cout
    std::cin.rdbuf(cin_buf);
    std::cout.rdbuf(cout_buf);

    // Check output contains expected results
    std::string out = output.str();
    assert(out.find("OK") != std::string::npos);
    assert(out.find("Gavin") != std::string::npos);
    assert(out.find("(nil)") != std::string::npos);

    std::cout << "RedisServer test passed!" << std::endl;
}

int main()
{
    test_redis_server();
    return 0;
}


#endif

#if 0
#include <threadPool/threadPool.hpp>
#include <threadPool/scheduler/PriorityScheduler.hpp>
#include <threadPool/scheduler/DAGschedule.hpp>
#include <threadPool/scheduler/FIFO_schedule.hpp>
#include <iostream>
#include "RedisServer.hpp"

int main() 
{
    try {
        // 1. Create ThreadPool
        ConcurrentEngine::ThreadPool pool;

        // 2. Attach a scheduler (FIFO, Priority, or DAG)
        pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());  
        // Or: pool.setScheduler(std::make_unique<PriorityScheduler>());

        // 3. Start the pool
        pool.start(4); // 4 worker threads

        // 4. Create mini-redis instance bound to this pool
        RedisServerConcurrent redis(pool);

        // 5. Run server loop
        redis.run();   // <--- this is your "miniredisConcurrent void run"
    }
    catch (const std::exception& ex) 
    {
        std::cerr << "Fatal: " << ex.what() << std::endl;
    }
}
#endif


#if 0

#include <iostream>
#include <threadPool/threadPool.hpp> 
#include <threadPool/logger/threadLogger.hpp>
#include "RedisServer.hpp"
#include "CommandParser.hpp"


int main() 
{
    ConcurrentEngine::ThreadPool pool;
    ThreadLogger::getInstance().enableConsoleLogging(false);

    // Set up FIFO scheduler
    pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
    pool.start(4); // Start with 4 threads

    RedisServer server(pool);
    CommandParser parser;

    std::string line;
    while(true) 
    {
        std::cout << "mini-Redis> ";
        std::getline(std::cin, line);
        Command cmd = parser.parse(line);

        auto result_future = server.submitCommand(cmd);
        std::string result = result_future.get();
        std::cout << result << std::endl;

        if(cmd.type == CommandType::QUIT) break;
    }

    return 0;
}


#endif 


#if 0

#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <threadPool/threadPool.hpp> 
#include <threadPool/logger/threadLogger.hpp>

#include "RedisServer.hpp"
#include "CommandParser.hpp"
#include "CommandExecutor.hpp"


Command randomCommand(int i) 
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0,3); // 0=SET,1=GET,2=DEL,3=EXISTS
    int cmd_type = dist(rng);
    std::string key = "key" + std::to_string(i);
    std::string value = "value" + std::to_string(i);

    switch(cmd_type) 
    {
        case 0: return {CommandType::SET, key, value};
        case 1: return {CommandType::GET, key, ""};
        case 2: return {CommandType::DEL, key, ""};
        case 3: return {CommandType::EXISTS, key, ""};
    }
    return {CommandType::INVALID, "", ""};
}

int main() 
{
    ConcurrentEngine::ThreadPool pool;
    ThreadLogger::getInstance().enableConsoleLogging(false);

    pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
    pool.start(16);

    RedisServer server(pool);
    CommandExecutor executor(server);

    const int N = 1000;            // 壓力測試命令數
    const int num_threads = 4;     // 模擬多線程 CLI 提交
    std::vector<std::thread> threads;
    std::vector<std::future<std::string>> results;
    results.reserve(N);

    auto start_time = std::chrono::steady_clock::now();

    for(int t=0; t<num_threads; t++) {
        threads.emplace_back([&]() {
            for(int i=t; i<N; i+=num_threads) 
            {
                Command cmd = randomCommand(i);
                auto fut = executor.executeCommand(cmd);
                // 加入結果保護
                {
                    static std::mutex mtx;
                    std::lock_guard<std::mutex> lock(mtx);
                    results.push_back(std::move(fut));
                }
            }
        });
    }

    for(auto &th : threads) th.join();

    // 等待所有結果並簡單檢查 SET/GET
    int success = 0;
    for(int i=0;i<results.size();i++)
    {
        std::string res = results[i].get();
        if(res != "ERR unknown command" && !res.empty()) success++;
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Total commands: " << results.size() << std::endl;
    std::cout << "Successful results: " << success << std::endl;
    std::cout << "Total time: " << duration_ms << " ms" << std::endl;

    return 0;
}



#endif 


#if 0
#include <iostream>
#include <threadPool/threadPool.hpp>
#include "RedisServer.hpp"

int main()
{
    try {
        // 1. 创建 ThreadPool
        ConcurrentEngine::ThreadPool pool;
        ThreadLogger::getInstance().enableConsoleLogging(false);
        pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
        pool.start(4); // 4 个 worker 线程

        // 2. 创建 mini-Redis 实例
        RedisServer redis(pool);

        // 3. 测试命令数组
        std::vector<std::string> commands = {
            "SET key1 hello world",
            "SET key2 123 456",
            "GET key1",
            "GET key2",
            "EXISTS key1 key2 key3",
            "DEL key1 key3",
            "GET key1",
            "PING",
            "HELP",
            "QUIT"
        };

        CommandParser parser;

        for (auto& line : commands) 
        {
            std::cout << "> " << line << std::endl;

            // 解析命令
            Command cmd = parser.parse(line);

            // QUIT 直接退出
            if (cmd.type == CommandType::QUIT)
            {
                std::cout << "Goodbye.. \n";
                break;
            }

            auto fut = redis.submitCommand(cmd);// 提交命令到线程池
            std::cout << fut.get() << std::endl;// 立即打印结果
        }

        pool.stop();

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
    }

    return 0;
}
#endif


#if 0 //light_way stress test
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <threadPool/threadPool.hpp>
#include "RedisServer.hpp"
#include "CommandParser.hpp"

// Helper to measure time
using Clock = std::chrono::high_resolution_clock;

int main() {
    try {
        // 1. 创建 ThreadPool
        ConcurrentEngine::ThreadPool pool;
        ThreadLogger::getInstance().enableConsoleLogging(false);
        
        // 可切换不同调度器: FIFO / Priority / DAG
        pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>());
        // pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::PriorityScheduler>());
        // pool.setScheduler(std::make_unique<ConcurrentEngine::Scheduler::DAGScheduler>());

        pool.start(4); // 4 worker 线程

        // 2. 创建 mini-Redis
        RedisServer redis(pool);
        CommandParser parser;

        // 3. 测试命令集合
        std::vector<std::string> commands = {
            "SET key1 hello world",
            "SET key2 123 456",
            "SET key3 foo bar baz",
            "GET key1",
            "GET key2",
            "GET key3",
            "EXISTS key1 key2 key4",
            "DEL key1 key4",
            "PING",
            "HELP"
        };

        std::vector<std::future<std::string>> results;
        results.reserve(commands.size());

        // 4. 提交命令并记录时间
        std::cout << "=== Submitting commands ===\n";
        for (auto& line : commands) {
            Command cmd = parser.parse(line);
            auto start = Clock::now();

            auto fut = redis.submitCommand(cmd);

            // 包装 future 打印延迟
            results.push_back(std::async(std::launch::async, [fut = std::move(fut), line, start]() mutable {
                auto res = fut.get();
                auto end = Clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                std::ostringstream oss;
                oss << "> " << line << " => " << res << "  [" << ms << " ms]";
                return oss.str();
            }));
        }

        // 5. 打印结果
        for (auto& fut : results) {
            std::cout << fut.get() << std::endl;
        }

        // 6. 停止线程池
        pool.stop();

        std::cout << "=== Test finished ===\n";

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
    }

    return 0;
}

#endif

#if 1 // with switching schedulers without DAG 
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <threadPool/threadPool.hpp>
#include "RedisServer.hpp"
#include "CommandParser.hpp"

using Clock = std::chrono::high_resolution_clock;

struct CommandStat {
    std::string commandLine;
    long long durationMs;
    std::string result;
};

int main() {
    try {
        // 命令批量
        std::vector<std::string> commands = {
            "SET key1 hello world",
            "SET key2 123 456",
            "SET key3 foo bar baz",
            "GET key1",
            "GET key2",
            "GET key3",
            "EXISTS key1 key2 key4",
            "DEL key1 key4",
            "PING",
            "HELP"
        };

        // 调度策略列表
        std::vector<std::pair<std::string, std::function<std::unique_ptr<ConcurrentEngine::Scheduler::IScheduler>()>>> schedulers = {
            {"FIFO", [](){ return std::make_unique<ConcurrentEngine::Scheduler::FIFOScheduler>(); }},
            {"Priority", [](){ return std::make_unique<ConcurrentEngine::Scheduler::PriorityScheduler>(); }}
        };


        for (auto& [name, factory] : schedulers) 
        {
            std::cout << "=== Testing Scheduler: " << name << " ===\n";

            ConcurrentEngine::ThreadPool pool;
            ThreadLogger::getInstance().enableConsoleLogging(false);
            pool.setScheduler(factory());
            pool.start(4);

            RedisServer redis(pool);
            CommandParser parser;
            std::vector<CommandStat> stats;

            for (auto& line : commands) {
                Command cmd = parser.parse(line);
                auto start = Clock::now();

                auto fut = redis.submitCommand(cmd);
                std::string result = fut.get();  // 同步获取结果

                auto end = Clock::now();
                long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                stats.push_back({line, duration, result});
            }

            // 打印结果表格
            long long total = 0, maxMs = 0;
            std::cout << "Command results:\n";
            for (auto& s : stats) 
            {
                std::cout << "> " << s.commandLine << " => " << s.result 
                          << " [" << s.durationMs << " ms]\n";
                total += s.durationMs;
                if (s.durationMs > maxMs) maxMs = s.durationMs;
            }
            std::cout << "Average latency: " << (total / stats.size()) << " ms, "
                      << "Max latency: " << maxMs << " ms\n\n";

            pool.stop();
        }

        std::cout << "=== All schedulers tested ===\n";

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
    }

    return 0;
}

#endif

#if 0 // DAG minimal example
#include <iostream>
#include <threadPool/threadPool.hpp>
#include <threadPool/scheduler/DAGschedule.hpp>
#include <memory>
#include <chrono>
#include <thread>

#include "RedisServer.hpp"
#include "CommandParser.hpp"


int main() {
    auto dagScheduler = std::make_unique<ConcurrentEngine::Scheduler::DAGScheduler>();
    ConcurrentEngine::ThreadPool pool(std::move(dagScheduler));
    pool.start(4);

    RedisServer redis(pool);

    Command setA{CommandType::SET, "key1", "hello"};
    Command setB{CommandType::SET, "key2", "world"};
    Command getA{CommandType::GET, "key1"};

    auto nodeA = redis.submitCommandDAG(setA); 
    auto nodeB = redis.submitCommandDAG(setB);
    auto nodeC = redis.submitCommandDAG(getA, {nodeA, nodeB}); // DAG依賴

    auto fut = redis.getResult(nodeC); // 外部才等待結果
    std::cout << "GET result = " << fut.get() << "\n";

    pool.stop();
}


#endif









