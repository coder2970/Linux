#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// 把一个进程变成守护进程
void Daemon()
{
    // 1.守护进程要把未来可能导致自己推出的信号忽略
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    // 2.让进程不再是组长
    if (fork() > 0)
        exit(0);
    // 3.子进程继承父进程,执行setsid,将自己设置为新会话
    setsid();
    // 4.子进程在父进程退出后变成后台进程,如何看待标准输入, 标准输出, 标准错误 ->重定向到 dev/null文件中
    // 将日志信息向文件写入
    int fd = open("/dev/null", O_RDWR);
    if (fd >= 0)
    {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
    }
}