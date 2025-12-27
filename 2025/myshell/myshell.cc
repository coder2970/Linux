#include "myshell.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 头文件声明，源文件实现
char *gargv[ARGS] = {NULL}; // 命令行参数表(子进程也要能拿到，故定义全局)
int gargc = 0;
char pwd[1028];  // 全局变量空间，保存当前shell进程的工作路径
int lastcode = 0;  // 获取程序退出码

// 测试
// void Debug()
// {
// 	printf("hello shell!\n");
// }
static std::string GetHomePath()
{
	std::string home = getenv("HOME");
	return home.empty() ? "/" : home;
}

static std::string GetUserName()
{		
	// 从环境变量中获取username hostname pwd
	std::string user = getenv("USER");
	return user.empty() ? "None" : user;
}

static std::string GetHostName()
{
	std::string hostname = getenv("HOSTNAME");
	return hostname.empty() ? "None" : hostname;
}

// Q1:getcwd()用法
// Q2:putenv()用法
// Q3:snprintf()用法
static std::string GetPwd()
{
	//环境变量的变化可能会依赖与进程，pwd需要shell自己更新环境变量的值
	// std::string pwd = getenv("PWD");
	// return pwd.empty() ? "None" : pwd;

	char temp[1024];
	getcwd(temp,sizeof(temp));  // 动态变化当前工作路径
	// 更新shell自己的环境变量
	snprintf(pwd,sizeof(pwd),"PWD=%s",temp);  // 把获取到的当前路径格式化到pwd中 
	putenv(pwd);  // 把pwd导入环境变量

	std::string pwd_lable = temp;
	const std::string pathsep = "/";
	auto pos = pwd_lable.rfind(pathsep);
	if(pos == std::string::npos) return "None";
	pwd_lable = pwd_lable.substr(pos + pathsep.size());


	return pwd_lable.empty() ? "/" : pwd_lable; 

}

// Q1:为什么用static修饰GetUsrName、GetHostName、GetPwd函数
// Q2：user.c_str()中为什么要用c_str函数转换
void PrintCommandPrompt()
{
	std::string user = GetUserName();
	std::string hostname = GetHostName();
	std::string pwd = GetPwd();

	printf("[%s@%s %s]# ", user.c_str(), hostname.c_str(), pwd.c_str());
}

// Q1:输入时可不可以用scanf或者cin,getline呢
// Q2:fgets()的用法
// Q3:如何解决输出时连续两次换行
// Q4:标准输入stdin是什么
bool GetCommandString(char cmd_str_buff[], int len)
{
	if (cmd_str_buff == NULL || len <= 0)
		return false;
	char *res = fgets(cmd_str_buff, len, stdin);
	if (res == NULL)
		return false;
	// 将最后一个回车\n换成\0  ls -a -l\n  --> ls -a -l\0
	cmd_str_buff[strlen(cmd_str_buff) - 1] = 0;
	// return true;
	return strlen(cmd_str_buff) == 0 ? false : true;
}

// Q1:strtok()的用法
// Q2:在没有使用InitGlobal接口时,gargc由于是全局变量而不会初始化,如何解决
bool ParseCommandString(char cmd[])
{
	// "ls -a -l"  --> "ls" "-a" "-l"
	if (cmd == NULL)
		return false;

#define SEP " "
	gargv[gargc++] = strtok(cmd, SEP);
	// 整个数组最后以NULL结尾
	while ((bool)(gargv[gargc++] = strtok(NULL, SEP)));
	// gargc多统计了一次 回退一次
	gargc--;

// #define DEBUG
#ifdef DEBUG
	printf("gargc: %d\n", gargc);
	printf("-------------------\n");
	for (int i = 0; i <gargc; i++)
	{
		printf("gargv[%d] : %s\n", i, gargv[i]);
	}
	printf("-------------------\n");

	for (int i = 0; gargv[i]; i++)
	{
		printf("gargv[%d] : %s\n", i, gargv[i]);
	}
#endif

	return true;
}

// Q1:memset的用法
void InitGlobal()
{
	gargc = 0;
	memset(gargv,0,sizeof(gargv));
}

bool BuiltInCommandExec()
{
	std::string cmd = gargv[0];
	bool ret = false;
	if(cmd == "cd")
	{
		// 检测之后参数，只有cd则返回家目录，cd ..则返回上级目录
		if(gargc == 2)
		{
			std::string target = gargv[1];
			if(target == "~")  // cd ~
			{
				chdir(GetHomePath().c_str());
				ret = true;
			}
			else  // cd /xxx/xxx/xxx
			{
				chdir(gargv[1]);
				ret = true;			
			}

		}
		else if(gargc == 1)
		{
			chdir(GetHomePath().c_str());
			ret = true;
		}
		else
		{
		}
	}
	else if(cmd == "echo")
	{
		if(gargc == 2)
		{
			std::string args = gargv[1];
			if(args[0] == '$')  // echo $? || echo $HOME
			{
				if(args[1] == '?')
				{
					printf("lastcode:%d\n",lastcode);
					lastcode = 0;
				}
				else
				{
					const char* ch = &args[1];
					printf("%s\n",getenv(ch));
					lastcode = 0;
				}
				ret = true;
			}
			else  // echo hello world
			{
				printf("%s\n",args.c_str());
				ret = true;
			}
		}
	}
	return ret;
}

// Q1:perror的用法
void ForkAndExec()
{
	pid_t id = fork();
	if(id < 0) 
	{
		perror("fork");
		return;
	}
	else if(id == 0)
	{
		execvp(gargv[0],gargv);
		exit(0);
	}
	else
	{
		int status = 0;
		pid_t rid = waitpid(id,&status,0);
		if(rid > 0)
		{
			lastcode = WEXITSTATUS(status);
		}		
	}

}
