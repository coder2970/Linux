#ifndef __MYSHELL_H__
#define __MYSHELL_H__

#include <stdio.h>

// 接收输入的参数 
#define ARGS 64

// 测试
// void Debug();

// 初始化
void InitGlobal();

// 打印命令行提示信息
void PrintCommandPrompt();

// 获取用户输入
// 用一个数组将输入的内容带出去
bool GetCommandString(char cmd_str_buff[],int len);

// 解析
bool ParseCommandString(char cmd[]);

// 检测內键命令
bool BuiltInCommandExec();

// 执行
void ForkAndExec();

#endif