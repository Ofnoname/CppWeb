// const int port = 8080;
// const int maxConnection = 16;

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 8080
#define DEFAULT_MODULE_PATH "modules/"
#define MAX_CONNECTION 16

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cond;
    bool stop = false;
public:
    ThreadPool(size_t threads);
    void enqueue(std::function<void()> task);
    ~ThreadPool();
};

class MultiThreadHttpServer
{
private:
    int server_fd, verbose;
    struct sockaddr_in address;
    ThreadPool threadPool;
    std::string modir;
public:
    MultiThreadHttpServer(int port, size_t numThreads, int verbose, const std::string& addressStr, const std::string& modir);
    void run();
};

typedef std::string (*module_generate_t)();
module_generate_t loadModule(const std::string& libPath);
void unloadModule(const std::string& libPath);

std::string Response(const std::string &content, const std::string &status = "200 OK");
std::string combinePaths(const std::string& base, const std::string& relative);

#endif // !SERVER_H