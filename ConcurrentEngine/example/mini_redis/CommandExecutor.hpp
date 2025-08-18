#ifndef COMMAND_EXECUTOR_HPP
#define COMMAND_EXECUTOR_HPP

#include <string>
#include "RedisDatabase.hpp"
#include "CommandParser.hpp"

class CommandExecutor 
{
public:
    explicit CommandExecutor(RedisDatabase& db) : db_(db) {}

    std::string execute(const std::string& cmd,
                        const std::string& key= "",
                        const std::string& value= "");

    std::string executeType(const Command& cmd);
    std::string commandTypeToString(const CommandType& cmdType);

private:
    RedisDatabase& db_;
};

#endif // COMMAND_EXECUTOR_HPP

