#ifndef REDIS_DATABASE_HPP
#define REDIS_DATABASE_HPP

#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>

class RedisDatabase 
{
public:
    bool set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);

private:
    std::unordered_map<std::string, std::string> data_;
    std::mutex mutex_;
};

#endif // REDIS_DATABASE_HPP

