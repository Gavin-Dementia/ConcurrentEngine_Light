#include "RedisServer.hpp"
#include <sstream>

RedisServer::RedisServer(ConcurrentEngine::ThreadPool& pool)
    : pool_(pool)
{}


std::future<std::string> RedisServer::submitCommand(const Command& cmd) 
{
    return pool_.submit("task", [this, cmd]() -> std::string {
        switch(cmd.type) 
        {
            case CommandType::SET:
            {
                db_.set(cmd.key, cmd.value);  // cmd.value 支持多单词
                return "OK";
            }
                
            case CommandType::GET:
            {  return db_.get(cmd.key);  }

            case CommandType::DEL:
            {
                std::istringstream iss(cmd.key);  // cmd.key 存所有 key
                std::string key;
                int deletedCount = 0;
                while (iss >> key)
                    if (db_.del(key) == "1") deletedCount++;
                return "(" + std::to_string(deletedCount) + ")";
            }

            case CommandType::EXISTS:
            {
                std::istringstream iss(cmd.key);  // cmd.key 存所有 key
                std::string key;
                int existCount = 0;
                while (iss >> key)
                    if (db_.get(key) == "1") existCount++;
                return "(" + std::to_string(existCount) + ")";
            }

            case CommandType::PING:
                return "PONG";

            case CommandType::QUIT:
                return "QUIT";

            case CommandType::HELP:
                return "Commands: GET, SET, DEL, EXISTS, PING, QUIT, HELP";

            default:
                return "ERR unknown command";
        }
    });
}







