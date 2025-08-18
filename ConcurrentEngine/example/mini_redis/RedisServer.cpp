#include "RedisServer.hpp"
#include <iostream>

void RedisServerConcurrent::run()
{
    std::string line;
    std::cout << "mini-Redis CLI (type QUIT to exit)\n";

    // Vector to hold pending futures
    std::vector<std::future<std::string>> pending;

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, line);

        Command cmd = parser_.parse(line);

        if (cmd.type == CommandType::QUIT)
            break;

        // Submit command using templated submit(), returns std::future<std::string>
        auto fut = pool_.submit("redis_task", [this, cmd]() {
            return executor_.executeType(cmd);
        });


        pending.push_back(std::move(fut));
    }

    // Print all results after commands are submitted
    for (auto& fut : pending)
        std::cout << fut.get() << std::endl;

    std::cout << "Goodbye!\n";
}




// void RedisServerConcurrent::run()
// {
//         std::string line;
//         std::cout << "mini-Redis CLI (type QUIT to exit)\n";

//         while (true)
//         {
//             std::cout << "> ";
//             std::getline(std::cin, line);

//             Command cmd = parser_.parse(line);

//             if (cmd.type == CommandType::QUIT)
//                 break;

//             // Submit command to ThreadPool
//             auto fut = pool_.submit([this, cmd]() {
//                 return executor_.executeType(cmd);
//             });

//             // Wait for result and print
//             std::cout << fut.get() << std::endl;
//             // std::vector<std::future<std::string>> pending;
//             // pending.push_back(pool_.submit([this, cmd]() {
//             //     return executor_.executeType(cmd);
//             // }));

//             // // Later: print all results
//             // for(auto &fut : pending)
//             //     std::cout << fut.get() << std::endl;
//             // pending.clear();

//         }

//         std::cout << "Goodbye!\n";
// }

// void RedisServer::run()
// {
//     std::string line;
//     std::cout << "mini-Redis CLI (type QUIT to exit)\n";

//     while (true)
//     {
//         std::cout << "> ";
//         std::getline(std::cin, line);

//         Command cmd = parser_.parse(line);

//         if (cmd.type == CommandType::QUIT)
//             break;

//         std::string result = executor_.executeType(cmd);
//         std::cout << result << std::endl;
//     }

//     std::cout << "Goodbye!\n";
// }
