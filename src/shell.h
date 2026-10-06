/*
 * shell.h - Causenix 交互式 Shell
 */

#pragma once

/* 初始化并进入 shell（不返回） */
void shell_init(void);

/* 解析并执行一条命令行（V3：网络命令入口；内置命令可安全在 timer 上下文调用） */
void shell_parse(char *cmdline);

/* 交互主循环：读键盘 -> 解析 -> 执行（不返回） */
void shell_run(void);
