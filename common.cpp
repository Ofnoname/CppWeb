#include "server.h"

std::string combinePaths(const std::string& base, const std::string& relative) {
    if (base.empty()) return relative;
    if (relative.empty()) return base;

    std::string combinedPath = base;

    // 如果base路径结尾有/，但relative路径开头没有/，则只添加relative
    // 如果base路径结尾没有/，且relative路径开头也没有/，则添加/
    // 其他情况，直接组合两者
    if (base.back() == '/' && relative.front() == '/') {
        combinedPath += relative.substr(1);
    } else if (base.back() != '/' && relative.front() != '/') {
        combinedPath += "/" + relative;
    } else {
        combinedPath += relative;
    }

    return combinedPath;
}

std::string Response(const std::string &content, const std::string &status)
{
    // 1. 打开template.html文件
    std::ifstream file("template.html");
    if (!file.is_open())
    {
        return "Error: Unable to open template.html";
    }

    // 2. 读取文件内容到一个字符串中
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string htmlContent = oss.str();

    // 3. 使用std::string::replace或其他方法替换{{content}}为content参数的值
    size_t pos = htmlContent.find("{{content}}");
    if (pos != std::string::npos)
    {
        htmlContent.replace(pos, 11, content); // 11 is the length of "{{content}}"
    }

    int contentLength = htmlContent.length();

    return
        R"(HTTP/1.1)" + status + R"(
        Content-Type: text/html
        Content-Length: )" +
        std::to_string(contentLength) + "\n\n" + htmlContent;
}