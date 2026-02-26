#pragma once
#include "Logger.hpp"
#include "Protocol.hpp"
#include "Calculator.hpp"
#include <iostream>
#include <string>
#include "functional"

using handler_t = std::function<Response(Request&)>;
// 解析报文
class Parser
{
public:
    Parser(handler_t handler) : _handler(handler)
    {
    }
    std::string Parse(std::string &inbuffer)
    {
        std::string send_str;
        // inbuffer可能包含多条完整报文, 这里一次性处理完, 所以使用while循环处理
        while (true)
        {
            std::string jsonstr;
            // 1.解析报文
            int n = Protocol::Unpack(inbuffer, &jsonstr);
            if (n < 0)
            {
                exit(0);
            }
            else if (n == 0)
            {
                break; // 已处理所有完整报文
            }
            else
            {
                LOG(LogLevel::DEBUG) << "\r\n" <<  jsonstr;
                Request request;
                // 2.反序列化
                if (!request.Deserialize(jsonstr))
                {
                    return std::string();
                }
                // 3.request -> response
                Response response = _handler(request);

                // 4.序列化返回
                std::string resp_json;
                if (!response.Serialize(&resp_json))
                {
                    return std::string();
                }

                // 5.打包 - 既然inbuffer可能包含多条报文, 那么返回时,也将多条报文的处理结果一并返回
                send_str += Protocol::Package(resp_json);
            }
        }
        // 6.完整报文,发回至客户端
        return send_str;
    }

    ~Parser()
    {
    }
private:
    handler_t _handler;
};