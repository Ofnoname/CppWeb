#include "server.h"

// class ThreadPool
// {

// private:
//     std::vector<std::thread> workers;
//     std::queue<std::function<void()>> tasks;
//     std::mutex mtx;
//     std::condition_variable cond;
//     bool stop = false;

// public:
ThreadPool::ThreadPool(size_t threads)
{
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back([this]
                             {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->mtx);
                        this->cond.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty()) return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                } });
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(task);
    }
    cond.notify_one();
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cond.notify_all();
    for (std::thread &worker : workers)
    {
        worker.join();
    }
}
// };

// class MultiThreadHttpServer
// {
// private:
//     int server_fd;
//     struct sockaddr_in address;
//     ThreadPool threadPool;

// public:
MultiThreadHttpServer::MultiThreadHttpServer(int port, size_t numThreads, int verbose, const std::string& addressStr, const std::string& modir)
    : threadPool(numThreads), verbose(verbose), modir(modir)
{
    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("创建Server错误：Failed to create socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
        // 如果addressStr为空，则使用INADDR_ANY，否则转换addressStr
    if (addressStr.empty()) {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        // Convert the IP address string to its binary form
        if (inet_pton(AF_INET, addressStr.c_str(), &(address.sin_addr)) <= 0) 
        {
            perror("创建Server错误：Invalid address or address not supported");
            exit(EXIT_FAILURE);
        }
    }
    
    address.sin_port = htons(port);

    // 绑定套接字到端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("创建Server错误：Bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听端口
    if (listen(server_fd, 10) < 0)
    {
        perror("创建Server错误：Listen failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "启动成功！服务器正运行在http://" 
          << (addressStr.empty() ? "localhost" : addressStr) 
          << ":" << port << std::endl;
}

void MultiThreadHttpServer::run()
{
    while (true)
    {
        int new_socket;
        int addrlen = sizeof(address);

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Server错误：Accept failed");
            exit(EXIT_FAILURE);
        }

        // 获取客户端的IP地址
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);



        threadPool.enqueue([=]()
                           {
            char buffer[3000] = {0};
            read(new_socket, buffer, 3000);

            // 只响应GET请求
            if (strncmp(buffer, "GET", 3) == 0) {


                // 1. 从HTTP请求中解析路径
                char path[256];  // Assuming path won't exceed 256 characters
                sscanf(buffer, "GET %s ", path);

                // Remove leading '/' from path
                std::string modulePath = path + 1;
                if (verbose)
                    std::cout << client_ip << "正在 GET 访问" << path << std::endl;
                if (modulePath.empty()) {
                    std::string response = Response("欢迎来到我的服务器");
                    send(new_socket, response.c_str(), response.length(), 0);
                }
                else if (modulePath == "favicon.ico") {
                    std::string response = Response("没有图标", "404 Not Found");
                    send(new_socket, response.c_str(), response.length(), 0);
                }
                else {
                    modulePath += ".so";  // Assuming shared libraries have .so extension
                    const auto actualModulePath = combinePaths(modir, modulePath);
                    const auto module_generate = loadModule(actualModulePath);

                    std::string response;
                    if (module_generate) {
                        response = Response(module_generate());
                        send(new_socket, response.c_str(), response.length(), 0);
                        unloadModule(actualModulePath);
                    } else {
                        std::cout << "没有可用模块 " << modulePath << std::endl;
                        response = Response("没有这个模块", "404 Not Found");
                        send(new_socket, response.c_str(), response.length(), 0);
                    }
                }

            } else {
                if (verbose)
                    std::cout << client_ip << "正在用其他方法访问" << std::endl;
                std::string response =  Response("不支持的请求", "400 Bad Request");
                send(new_socket, response.c_str(), response.length(), 0);
            }
            close(new_socket); });
    }
}
// };