#include "common.hpp"

const int defaultfd = -1;

class NamedPipe
{
public:
    NamedPipe(const std::string &name)
        : _fd(defaultfd), _name(name)
    {
    }

    bool Creat()
    {
        // 创建管道文件
        int n = mkfifo(fifoname.c_str(), mode);
        if (n == 0)
        {
            std::cout << "创建成功 " << std::endl;
        }
        else
        {
            perror("mkfifo:");
            return false;
        }
        return true;
    }

    void Close()
    {
        if (_fd == defaultfd)
            return;
        else
            close(_fd);
    }

    bool OpenForRead()
    {
        _fd = open(fifoname.c_str(), O_RDONLY);
        if (_fd < 0)
        {
            perror("open");
            return false;
        }
        std::cout << "文件打开成功" << std::endl;
        return true;
    }

    bool OpenForWrite()
    {
        _fd = open(fifoname.c_str(), O_WRONLY);
        if (_fd < 0)
        {
            perror("open");
            return false;
        }
        return true;
    }
    // 输入型参数 ：const&
    //  输出型参数：*
    //  输入输出型参数：&
    bool Read(std::string *out)
    {
        char buffer[SIZE] = {0};
        ssize_t n = read(_fd, buffer, sizeof(buffer) - 1);
        if (n < 0)
        {
            perror("read");
            return false;
        }
        else if (n == 0)
        {
            return false;
        }
        else
        {
            buffer[n] = 0;
            *out = buffer;
        }
        return true;
    }

    void Write(const std::string &in)
    {
        write(_fd, in.c_str(), in.size());
    }

    void Remove()
    {
        int m = unlink(fifoname.c_str());
        (void)m;
    }

    ~NamedPipe()
    {
    }

private:
    int _fd;
    std::string _name;
};