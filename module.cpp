#include <dlfcn.h>

#include "server.h"

typedef std::string (*module_generate_t)();

// 加载动态库并返回module_generate函数
module_generate_t loadModule(const std::string& libPath) {
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << '\n';
        return nullptr;
    }

    // 清除现有的错误
    dlerror();

    module_generate_t moduleFunction = (module_generate_t) dlsym(handle, "module_generate");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol 'module_generate': " << dlsym_error << '\n';
        dlclose(handle);
        return nullptr;
    }

    return moduleFunction;
}

// 卸载动态库
void unloadModule(const std::string& libPath) {
    void* handle = dlopen(libPath.c_str(), RTLD_NOW);
    if (handle) {
        dlclose(handle);
    }
}