#ifndef REDISSERVER_HPP
#define REDISSERVER_HPP

#include <future>
#include <threadPool/threadPool.hpp> 
#include "RedisDatabase.hpp"
#include "CommandParser.hpp"

class RedisServer 
{
public:
    RedisServer(ConcurrentEngine::ThreadPool& pool);
    std::future<std::string> submitCommand(const Command& cmd);

private:
    RedisDatabase db_;
    ConcurrentEngine::ThreadPool& pool_;
};

#endif // REDISSERVER_HPP

