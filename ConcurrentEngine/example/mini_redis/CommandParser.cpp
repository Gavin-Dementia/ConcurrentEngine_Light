#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>

Command CommandParser::parse(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmdStr;
    iss >> cmdStr;

    std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::toupper);

    if (cmdStr == "GET") 
    {
        std::string key;
        iss >> key;
        return {CommandType::GET, key, ""};
    }

    if (cmdStr == "SET") 
    {
        std::string key;
        iss >> key;
        std::string value;
        std::getline(iss, value);           // 读取剩下的整行作为 value
        if (!value.empty() && value[0] == ' ')
            value.erase(0, 1);              // 去掉前导空格
        return {CommandType::SET, key, value};
    }

    if (cmdStr == "DEL") 
    {
        std::string key;
        iss >> key;
        return {CommandType::DEL, key, ""};
    }

    if (cmdStr == "EXISTS") 
    {
        std::string key;
        iss >> key;
        return {CommandType::EXISTS, key, ""};
    }

    if (cmdStr == "PING") return {CommandType::PING, "", ""};
    if (cmdStr == "QUIT") return {CommandType::QUIT, "", ""};
    if (cmdStr == "HELP") return {CommandType::HELP, "", ""};

    return {CommandType::INVALID, "", ""};
}
