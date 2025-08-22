#ifndef COMMAND_EXECUTOR_HPP
#define COMMAND_EXECUTOR_HPP

#include <future>
#include "CommandParser.hpp"
#include "RedisServer.hpp"

class CommandExecutor 
{
public:
    CommandExecutor(RedisServer& server);

    // 接收 Command，提交到 ThreadPool
    std::future<std::string> executeCommand(const Command& cmd);

private:
    RedisServer& server_;
};

#endif // COMMAND_EXECUTOR_HPP

