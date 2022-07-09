#ifndef __SHELL_H__
#define __SHELL_H__

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <pwd.h>
#include<time.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>


// 读取字符串
char* read_line();

// 进入 shell 循环处理
int loop();

// 提取指令中的所有参数
char ** split(char* instruction);

// pwd 获取当前工作路径
int pwd_function(char **tokens);

// cd 切换工作路径
int cd_function(char** tokens);

// date 获取当前系统时间
int date_function(char **tokens);

// echo 回显
int echo_function(char **tokens);

// help 展示内部命令
int help(char **tokens);

// 调用外部进程
int outer_process(char ** tokens);

// exit 退出 shell 程序
int exit_function(char **tokens);

// 获取用户名
char *get_user_name();

// 执行的接口
int execute(char **tokens);

// ls 查看当前目录下所有文件
int ls_function(char **tokens);

// pipe 管道
int pipe_function(char *line);

// redirect 重定向
int redirect_function(char * line);

#endif