#include "CommandExecutor.hpp"
#include <algorithm>

std::string CommandExecutor::execute(const std::string& cmd,
                                     const std::string& key,
                                     const std::string& value) 
{
    std::string command = cmd;
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "SET") 
    {
        db_.set(key, value);
        return "OK";
    }
    else if (command == "GET") 
    {
        auto result = db_.get(key);
        return result.has_value() ? result.value() : "(nil)";
    }
    else if (command == "DEL") 
    {
        bool deleted = db_.del(key);
        return deleted ? "(1)" : "(0)";
    }
    else if (command == "EXISTS")
    {
        bool exists = db_.get(key).has_value();
        return exists ? "(1)" : "(0)";
    }
    else if (command == "HELP") 
        return "Supported commands: SET key value | GET key | DEL key | HELP";
    else
        return "ERR unknown command: " + cmd;
    

    return "(nil)"; // or "UNKNOWN COMMAND"
}

std::string CommandExecutor::executeType(const Command& cmd) 
{  return execute(commandTypeToString(cmd.type), cmd.key, cmd.value);  }

std::string CommandExecutor::commandTypeToString(const CommandType& cmdType)
{
    switch (cmdType) 
    {
        case CommandType::GET: return "GET";
        case CommandType::SET: return "SET";
        case CommandType::DEL: return "DEL";
        case CommandType::EXISTS: return "EXISTS";
        case CommandType::PING: return "PING";
        case CommandType::QUIT: return "QUIT";
        case CommandType::HELP: return "HELP";
        default: return "INVALID";
    }
}




