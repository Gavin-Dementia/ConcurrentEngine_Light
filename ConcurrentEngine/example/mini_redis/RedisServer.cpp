#include "RedisServer.hpp"

RedisServer::RedisServer(ConcurrentEngine::ThreadPool& pool) 
        : pool_(pool) 
    {}

std::future<std::string> RedisServer::submitCommand(const Command& cmd) {
    return pool_.submit("task", [this, cmd]() -> std::string {
        switch(cmd.type) 
        {
            case CommandType::GET: return db_.get(cmd.key);
            case CommandType::SET: return db_.set(cmd.key, cmd.value);
            case CommandType::DEL: return db_.del(cmd.key);
            case CommandType::EXISTS: return db_.exists(cmd.key);
            case CommandType::PING: return "PONG";
            case CommandType::QUIT: return "QUIT";
            case CommandType::HELP: return "Commands: GET, SET, DEL, EXISTS, PING, QUIT, HELP";
            default: return "ERR unknown command";
        }
    });
}

