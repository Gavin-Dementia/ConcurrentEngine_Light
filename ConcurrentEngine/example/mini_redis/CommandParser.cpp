#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>

Command CommandParser::parse(const std::string& line) 
{
    std::istringstream iss(line);
    std::string cmdStr, key, value;
    iss >> cmdStr;

    std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::toupper);

    if(cmdStr == "GET") { iss >> key; return {CommandType::GET, key, ""}; }
    if(cmdStr == "SET") { iss >> key >> value; return {CommandType::SET, key, value}; }
    if(cmdStr == "DEL") { iss >> key; return {CommandType::DEL, key, ""}; }
    if(cmdStr == "EXISTS") { iss >> key; return {CommandType::EXISTS, key, ""}; }
    if(cmdStr == "PING") return {CommandType::PING, "", ""};
    if(cmdStr == "QUIT") return {CommandType::QUIT, "", ""};
    if(cmdStr == "HELP") return {CommandType::HELP, "", ""};
    return {CommandType::INVALID, "", ""};
}
