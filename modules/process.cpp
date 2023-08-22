#include <string>
#include <dirent.h>

extern "C"
{
std::string module_generate()
{
    int count = 0;
    DIR *dir;
    if ((dir = opendir("/proc")) != NULL)
    {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL)
        {
            if (isdigit(ent->d_name[0]))
            {
                count++;
            }
        }
        closedir(dir);
    }
    return "当前的进程数是：" + std::to_string(count);
}
}
