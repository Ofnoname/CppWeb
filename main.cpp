#include "server.h"

int main(int argc, char* argv[]) {
    int opt;
    std::string address = "", modir = DEFAULT_MODULE_PATH;
    int port = DEFAULT_PORT;
    bool verbose = true;

    // 定义短选项格式
    const char* optstring = "a:hm:p:v";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'a':
                address = optarg;
                break;
            case 'h':
                std::cout << "Help: \n";
                std::cout << "-a/--address: 指定服务器地址，默认 127.0.0.1 \n";
                std::cout << "-h/--help: 显示帮助\n";
                std::cout << "-m/--modir: 设定模块的目录，默认在 module 文件夹下 \n";
                std::cout << "-p/--port: 设定服务器端口，默认在 8080\n";
                std::cout << "-v/--verbose: 是否输出运行信息，默认开启\n";
                return 0;
            case 'm':
                modir = optarg;
                break;
            case 'p':
                port = std::stoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            default:
                std::cerr << "Unknown option: " << opt << "\n";
                return 1;
        }
    }

    MultiThreadHttpServer server(port, MAX_CONNECTION, verbose, address, modir);
    server.run();
    return 0;
}
