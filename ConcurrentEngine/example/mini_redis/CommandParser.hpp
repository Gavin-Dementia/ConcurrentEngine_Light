#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <sstream>

enum class CommandType { GET, SET, DEL, EXISTS, PING, QUIT, INVALID, HELP };

struct Command { CommandType type; std::string key, value; };

class CommandParser
{
public:
    Command parse(const std::string& line);

private:
    Command parseGetCommand(const std::string& line);
    Command parseSetCommand(const std::string& line);
    Command parseDelCommand(const std::string& line);
    Command parseExistsCommand(const std::string& line);
    Command parsePingCommand(const std::string& line);
    Command parseQuitCommand(const std::string& line);
    Command parseInvalidCommand(const std::string& line);
    Command parseHelpCommand(const std::string& line);

};

#endif // COMMAND_PARSER_HPP


