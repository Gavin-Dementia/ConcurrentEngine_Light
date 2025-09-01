#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>

enum class CommandType { GET, SET, DEL, EXISTS, PING, QUIT, HELP, INVALID };
struct Command { CommandType type; std::string key, value; };

class CommandParser 
{
public:
    Command parse(const std::string& line);
};

#endif // COMMAND_PARSER_HPP

