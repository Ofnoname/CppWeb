#include <string>
#include <fstream>

extern "C"
{
std::string module_generate()
{
    std::ifstream file("/etc/os-release");
    std::string line;
    while (std::getline(file, line))
    {
        if (line.find("PRETTY_NAME=") == 0)
        {
            return "当前的系统版本是：" + line.substr(13, line.size() - 14); // Remove "PRETTY_NAME=" and quotes
        }
    }
    return "Unknown";
}
}

