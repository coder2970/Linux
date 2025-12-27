#include "namedpipe.hpp"

int main()
{
    NamedPipe named_pipe(fifoname);
    named_pipe.Creat();
    named_pipe.OpenForRead();

    // 打开文件
    std::string message;
    while (true)
    {
        bool res = named_pipe.Read(&message);
        if (!res)
            break;
        std::cout << "client say # " << message << std::endl;
    }

    // 归还资源
    named_pipe.Close();
    named_pipe.Remove();
    return 0;
}