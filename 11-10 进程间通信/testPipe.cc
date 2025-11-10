// #include <iostream>
// #include <cstdio>
// #include <unistd.h>

// // int main()
// // {
// //     int pipefd[2] = {0};
// //     int n = pipe(pipefd);
// //     if (n == 0)
// //     {
// //         printf("create pipe success! pipefd[0] : %d,pipefd[1] : %d\n", pipefd[0], pipefd[1]);
// //     }
// //     return 0;
// // }

// 父子通信 子进程写 父进程读
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>

int main()
{
    // 1.父进程创建管道
    int pipefd[2] = {0};
    int n = pipe(pipefd);
    (void)n;

    // 2.fork创建子进程
    pid_t id = fork();
    if (id < 0)
    {
        perror("fork:");
        exit(1);
    }
    else if (id == 0)
    {
        // 子进程
        // 3.父子关闭不需要的fd
        close(pipefd[0]);

        std::string str = "hello father i m child process";
        std::string self = std::to_string(getpid());
        int cnt = 1;
        while (cnt)
        {

            // sleep(500);
            // std::string message = str + " " + self + " ";
            // message += std::to_string(cnt++);
            // message += "\r\n";
            // // 把message信息写入
            // write(pipefd[1], message.c_str(), message.size());
            // // sleep(1);
            sleep(1);
            char ch = 'A' + cnt;
            write(pipefd[1], &ch, 1);
            // cnt++;
            std::cout << "count :" << cnt++ << std::endl;
        }
        // close(pipefd[1]);
        // std::cout << "write close" << std::endl;
        // sleep(5);
        // exit(0);
    }
    else
    {
        // 父进程
        // 3.父子关闭不需要的fd
        close(pipefd[1]);
        // close(pipefd[0]);
        while (true)
        {

            // sleep(1);
            // // sleep(5);
            // char inbuffer[1024] = {0};
            // ssize_t n = read(pipefd[0], inbuffer, sizeof(inbuffer) - 1); // 预留一个位置给\0
            // if (n > 0)
            // {
            //     inbuffer[n] = 0;
            //     std::cout << "client->father# " << inbuffer << std::endl;
            // }

            char c;
            ssize_t n = read(pipefd[0], &c, 1);
            if (n > 0)
            {
                std::cout << "读到数据" << c << std::endl;
            }
            else if (n == 0)
            {
                std::cout << "return val : " << n << std::endl;
                sleep(1);
            }
            else
                std::cout << "read error" << std::endl;
            break;
        }
        close(pipefd[0]);
        std::cout << "读端关闭" << std::endl;

        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        if (rid > 0)
        {
            int exitcode = (status >> 8) & 0xFF;
            int exitsig = status & 0x7F;
            std::cout << "wait success exitcode : " << exitcode << " exitsig:" << exitsig << std::endl;
        }
        (void)rid;
    }

    // 4.正常通信传递数据

    return 0;
}
