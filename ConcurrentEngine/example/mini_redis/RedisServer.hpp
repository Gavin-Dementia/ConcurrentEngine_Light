#ifndef REDISSERVER_HPP
#define REDISSERVER_HPP

#if 0
#include "CommandParser.hpp"
#include "CommandExecutor.hpp"
#include <string>

class RedisServer
{
public:
    RedisServer();
    void run();  // main loop

private:
    RedisDatabase db_;
    CommandParser parser_;
    CommandExecutor executor_;
};

#endif 

#if 1
#include "CommandParser.hpp"
#include "CommandExecutor.hpp"
#include <threadPool/threadPool.hpp> 

#include <iostream>
#include <future>
#include <vector>

class RedisServerConcurrent
{
public:
    RedisServerConcurrent(ConcurrentEngine::ThreadPool& pool)
        : executor_(db_)
        , pool_(pool)
    {}

    void run();

private:
    RedisDatabase db_;
    CommandParser parser_;
    CommandExecutor executor_;
    ConcurrentEngine::ThreadPool& pool_;
};


#endif

#endif // REDISSERVER_HPP

