#include "myshell.h"

#define SIZE 1024

int main()
{
	char commandstr[SIZE]; // 用户命令行输入
	// Debug();
	while (true)
	{
		// 0.初始化操作
		InitGlobal();

		// 1.输出命令行提示符
		PrintCommandPrompt();

		// 2.获取用户输入命令
		if (!GetCommandString(commandstr, SIZE))
			continue;
		// printf("echo %s\n",commandstr);

		// 3.对字符串进行解析 "ls -a -l"  --> "ls" "-a" "-l" 解析成命令行参数表(子进程也要能拿到，故定义全局)
		ParseCommandString(commandstr);
		// (暂时)4.执行命令 子进程执行
		// ForkAndExec();



		// 4.检测命令 检测内键命令 -> 内键命令让父进程自己执行
		if(BuiltInCommandExec()) continue;

		// 5.执行命令
		ForkAndExec();
	}
	return 0;
}
