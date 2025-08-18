#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>

Command CommandParser::parse(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    // 大寫轉小寫（如果你之後需要處理大小寫不敏感）
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (cmd == "GET")
        return parseGetCommand(line);
    else if (cmd == "SET")
        return parseSetCommand(line);
    else if (cmd == "DEL")
        return parseDelCommand(line);
    else if (cmd == "EXISTS")
        return parseExistsCommand(line);
    else if (cmd == "PING")
        return parsePingCommand(line);
    else if (cmd == "QUIT")
        return parseQuitCommand(line);
    else if (cmd == "HELP")
        return parseHelpCommand(line);

    return parseInvalidCommand(line);
}

Command CommandParser::parseGetCommand(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmd, key;
    iss >> cmd >> key;

    if(key.empty()) 
        return parseInvalidCommand(line);

    return Command{ CommandType::GET, key, "" };
}

Command CommandParser::parseSetCommand(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmd, key, value;
    iss >> cmd >> key >> value;

    if(key.empty() || value.empty()) 
        return parseInvalidCommand(line);
    
    return Command{ CommandType::SET, key, value };
}

Command CommandParser::parseDelCommand(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmd, key;
    iss >> cmd >> key;

    if(key.empty()) 
        return parseInvalidCommand(line);

    return Command{ CommandType::DEL, key, "" };
}

Command CommandParser::parseInvalidCommand(const std::string& line) 
{  return Command{ CommandType::INVALID, "", "" };  }

Command CommandParser::parseExistsCommand(const std::string& line)
{
    std::istringstream iss(line);
    std::string cmd, key;
    iss >> cmd >> key;

    if (key.empty())
        return parseInvalidCommand(line);

    return Command{ CommandType::EXISTS, key, "" };
}

Command CommandParser::parsePingCommand(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmd, key;
    iss >> cmd >> key;

    if (!key.empty() && cmd != "PING") 
        return parseInvalidCommand(line);

    return Command{ CommandType::PING, "", "" };
}

Command CommandParser::parseQuitCommand(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmd, key;
    iss >> cmd >> key;

    if (!key.empty() && cmd != "QUIT") 
        return parseInvalidCommand(line);

    return Command{ CommandType::QUIT, "", "" };
}

Command CommandParser::parseHelpCommand(const std::string& line)
{
    std::istringstream iss(line);
    std::string cmd, key;
    iss >> cmd >> key;

    if (!key.empty() && cmd != "HELP") 
        return parseInvalidCommand(line);

    return Command{ CommandType::HELP, "", "" };
}

