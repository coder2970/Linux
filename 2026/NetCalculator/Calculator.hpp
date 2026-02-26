#pragma once

#include "Protocol.hpp"
#include <iostream>
#include <string>

// 进行计算
class Calculator
{
public:
    Calculator() {}
    Response Exec(Request &request)
    {
        Response response;
        switch (request.Oper())
        {
        case '+':
            response.SetResult(request.X() + request.Y());
            break;
        case '-':
            response.SetResult(request.X() - request.Y());
            break;
        case '*':
            response.SetResult(request.X() * request.Y());
            break;
        case '/':
            if (request.Y() == 0)
            {
                response.SetCode(1);
            }
            else
            {
                response.SetResult(request.X() / request.Y());
            }
            break;
        case '%':
            if (request.Y() == 0)
            {
                response.SetCode(2);
            }
            else
            {
                response.SetResult(request.X() % request.Y());
            }
            break;

        default:
            response.SetCode(3);
            break;
        }
        return response;
    }

    ~Calculator() {}
};