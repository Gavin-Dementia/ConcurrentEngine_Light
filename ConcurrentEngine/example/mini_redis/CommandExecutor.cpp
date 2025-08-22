#include "CommandExecutor.hpp"

CommandExecutor::CommandExecutor(RedisServer& server)
        : server_(server) 
    {}

std::future<std::string> CommandExecutor::executeCommand(const Command& cmd) 
{
    // 可以在這裡加入額外邏輯，例如：
    // - 日誌 log
    // - 驗證指令合法性
    // - 指令優先級判斷
    return server_.submitCommand(cmd);
}

