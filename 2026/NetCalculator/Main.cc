#include "Calculator.hpp" // 业务
#include "Parser.hpp"     // 报文解析, 序列化与反序列化
#include "TcpServer.hpp"  // 通信IO, 开断连接
#include "Daemon.hpp"
#include <memory>

void Usage(std::string s)
{
    std::cerr << "Usage: " << s << " local_port" << std::endl;
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    Daemon();
    // EnableConsoleLogStragegy();
    EnableFileLogStragegy();
    // 1.计算器对象
    std::unique_ptr<Calculator> cal = std::make_unique<Calculator>();
    // 2.协议解析模块
    std::unique_ptr<Parser> parser = std::make_unique<Parser>([&cal](Request &request) -> Response
                                                              { return cal->Exec(request); });
    // 3.网络通信模块
    uint16_t server_port = std::stoi(argv[1]);
    std::unique_ptr<TcpServer> tcp_socket = std::make_unique<TcpServer>(server_port, [&parser](std::string &inbuffer) -> std::string
                                                                        { return parser->Parse(inbuffer); });
    tcp_socket->Run();
    return 0;
}