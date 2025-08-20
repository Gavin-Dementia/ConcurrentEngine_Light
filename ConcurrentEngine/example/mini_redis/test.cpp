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

#if 1
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

