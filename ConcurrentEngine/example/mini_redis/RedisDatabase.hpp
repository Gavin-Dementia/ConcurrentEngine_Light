#ifndef REDIS_DATABASE_HPP
#define REDIS_DATABASE_HPP

#include <unordered_map>
#include <string>
#include <mutex>

class RedisDatabase {
public:
    std::string get(const std::string& key);
    std::string set(const std::string& key, const std::string& value);
    std::string del(const std::string& key);
    std::string exists(const std::string& key);

private:
    std::unordered_map<std::string, std::string> store_;
    std::mutex mutex_;
};


#endif // REDIS_DATABASE_HPP

