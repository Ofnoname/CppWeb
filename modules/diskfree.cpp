#include <string>
#include <sys/statvfs.h>

extern "C"
{
std::string module_generate()
{
    struct statvfs buf;
    if (statvfs("/", &buf) == 0)
    {
        return "磁盘剩余容量是：" + std::to_string(buf.f_bfree * buf.f_bsize / (1024 * 1024)) + " MB";
    }
    return "Unknown";
}
}

