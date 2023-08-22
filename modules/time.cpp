#include <string>
#include <chrono>
#include <ctime>

extern "C"
{
    std::string module_generate()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        return "现在的服务器时间是：" + (std::string)std::ctime(&now_time);
    }
}
