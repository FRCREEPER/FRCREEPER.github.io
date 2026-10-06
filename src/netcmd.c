/*
 * netcmd.c - 环形命令队列（无锁：单生产者 timer 轮询 / 单消费者 shell_run）
 */

#include "netcmd.h"
#include "printk.h"
#include "shell.h"

static char     g_q[NETCMD_MAX][NETCMD_LEN];
static uint32_t g_head;   /* 消费者读出位置 */
static uint32_t g_tail;   /* 生产者写入位置 */

int netcmd_enqueue(const char *cmd, uint32_t len)
{
    if (!cmd || len == 0) return -1;
    if (len >= NETCMD_LEN) len = NETCMD_LEN - 1;   /* 截断保 NUL */
    uint32_t next = (g_tail + 1) % NETCMD_MAX;
    if (next == g_head) {                          /* 队列满 */
        printk("[net-cmd] queue full, drop %u B\n", len);
        return -1;
    }
    for (uint32_t i = 0; i < len; i++) g_q[g_tail][i] = cmd[i];
    g_q[g_tail][len] = '\0';
    g_tail = next;
    return 0;
}

int netcmd_dequeue(char *out, uint32_t max)
{
    if (g_head == g_tail) return -1;               /* 空 */
    uint32_t i = 0;
    while (i < max - 1 && g_q[g_head][i] != '\0') {
        out[i] = g_q[g_head][i];
        i++;
    }
    out[i] = '\0';
    g_head = (g_head + 1) % NETCMD_MAX;
    return 0;
}

/* 内置命令白名单（与 shell_exec 的 else-if 链一致；排除 reboot：网络触发重启危险）。
 * 不含 '/' —— 程序命令（/bin/xxx）会 user_run 切用户态，不能在中断上下文执行。 */
static int is_builtin(const char *name)
{
    static const char *tab[] = {
        "help", "echo", "version", "mem", "pwd", "clear",
        "ls", "cat", "fsck", "gc", "mount", "mkdir", "rm", "tree", "cd",
    };
    for (uint32_t i = 0; i < sizeof(tab) / sizeof(tab[0]); i++) {
        const char *a = name, *b = tab[i];
        while (*a && *b && *a == *b) { a++; b++; }
        if (*a == '\0' && *b == '\0') return 1;
    }
    return 0;
}

void netcmd_drain(void)
{
    char cmd[NETCMD_LEN];
    while (netcmd_dequeue(cmd, sizeof(cmd)) == 0) {
        /* 取首词 */
        char *sp = cmd;
        while (*sp == ' ') sp++;
        char *end = sp;
        while (*end && *end != ' ') end++;
        char save = *end;
        *end = '\0';
        int builtin = is_builtin(sp);
        *end = save;

        if (!builtin) {
            printk("[net-cmd] drop non-builtin: %s\n", sp);
            continue;
        }
        printk("[net-cmd] %s\n", cmd);
        shell_parse(cmd);
    }
}
