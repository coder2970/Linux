#include "DictServer.hpp" // 负责网络IO
#include "Dictionary.hpp" // 负责翻译
#include <iostream>
#include <memory>

void Usage(std::string message)
{
    std::cerr << "usage:" << message << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    EnableConsoleLogStragegy(); // 启用控制台日志刷新策略 - 向显示器打印日志

    Dictionary dict("./Dict.txt");
    uint16_t port = std::stoi(argv[1]);

    // 创建对象
    std::unique_ptr<DictServer> usvr = std::make_unique<DictServer>(port,
                                                                    [&dict](const std::string &word, const std::string &who_ip, uint16_t who_port) -> std::string
                                                                    { return dict.Translate(word, who_ip, who_port); });

    usvr->Init(); // 初始化

    usvr->Start(); // 启动
    return 0;
}