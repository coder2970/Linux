#include "process_pool.hpp"

int main()
{
    // 1.创建进程池
    ProcessPool pp(5);
    // 2.初始化进程池
    pp.InitProcessPool([](int fd)
                       {
        while(true)
        {
            int code = 0;
            ssize_t n = read(fd,&code,sizeof(code));
            if (n == sizeof(code))
            {
                std::cout << "子进程被唤醒: " << getpid() << std::endl;
                if(code >= 0 && code < tasks.size())
                {
                    tasks[code]();
                }
                else
                {
                    std::cerr << "父进程给的任务码错误: " << code << std::endl;
                }
            }
            else if (n == 0)
            {
                std::cout << "子进程该退出了: " << getpid() << std::endl;
                break;
            }
            else
            {
                std::cerr << "read错误: " << fd << std::endl;
                break;
            }
        } });
    // 3.控制进程池
    pp.PollingCtrlProcess(10);
    // 4.结束进程池
    pp.WaitSubProcess();
    std::cout << "父进程控制子进程完成，父进程结束" << std::endl;
    return 0;
}