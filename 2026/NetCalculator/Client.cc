#include "Socket.hpp"
#include "InetAddr.hpp"
#include "Protocol.hpp"
#include <iostream>
#include <string>
#include <memory>

void Usage(const std::string message)
{
    std::cerr << "Usage : " << message << " server_ip server_port" << std::endl;
}
// ./Client server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    std::unique_ptr<Socket> tcp_socket = std::make_unique<TcpSocket>();
    tcp_socket->BuildClientSocketMethod();
    InetAddr server(serverport, serverip);
    if (tcp_socket->Connect(server))
    {
        std::string inbuffer;
        while (true)
        {
            sleep(1);
            // 构建请求
            Request request;
            std::cout << "please enter x: ";
            std::cin >> request._x;
            std::cout << "please enter y: ";
            std::cin >> request._y;
            std::cout << "please enter operator(+-*/%): ";
            std::cin >> request._oper;

            // 序列化
            std::string jsonstr;
            request.Serialize(&jsonstr);

            // 打包
            std::string sendstr = Protocol::Package(jsonstr);

            // 发送
            tcp_socket->Send(sendstr);

            // 接收
            tcp_socket->Recv(&inbuffer);
            
            // 报文解析
            std::string package;
            int n = Protocol::Unpack(inbuffer, &package);
            if(n > 0)
            {
                // 反序列化
                Response response;
                bool r = response.Deserialize(package);
                if(r)
                {
                    response.Print();
                }
            }
        }
    }

    return 0;
}