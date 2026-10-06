/*
 * netcmd.h - 网络命令队列（V3：UDP payload → shell 命令）
 *
 * 设计：timer 轮询（中断上下文）只做 IP/UDP 解析 + 入队；
 * shell_run 主循环（安全上下文）消费并执行，避免在中断里跑用户程序。
 */

#pragma once

#include <stdint.h>

#define NETCMD_MAX  64        /* 队列深度 */
#define NETCMD_LEN  256       /* 单条命令最大长度（含 NUL） */

/* 入队一条命令（拷贝 + NUL 结尾）。len 为 payload 字节数。
 * 返回 0 成功；-1 队列满；-2 超长截断。可在中断上下文调用。 */
int netcmd_enqueue(const char *cmd, uint32_t len);

/* 取出一条命令（含 NUL 结尾）。返回 0 成功；-1 空队列。 */
int netcmd_dequeue(char *out, uint32_t max);

/* 消费队列直到空：内置白名单命令 → shell_parse 执行，其余丢弃并打印。
 * 设计：仅由 timer 轮询上下文调用（生产者 handle_ipv4 同上下文，无需锁）。 */
void netcmd_drain(void);
