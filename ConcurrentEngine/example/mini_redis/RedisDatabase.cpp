#include "RedisDatabase.hpp"

std::string RedisDatabase::get(const std::string& key) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = store_.find(key);
    return it != store_.end() ? it->second : "(nil)";
}

std::string RedisDatabase::set(const std::string& key, const std::string& value) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    store_[key] = value;
    return "OK";
}

std::string RedisDatabase::del(const std::string& key) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    return store_.erase(key) > 0 ? "1" : "0";
}

std::string RedisDatabase::exists(const std::string& key) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    return store_.count(key) > 0 ? "1" : "0";
}

