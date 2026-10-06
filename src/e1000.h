/*
 * e1000.h - Intel e1000 (82540EM) 网卡驱动（V1：收包 + dump）
 */

#pragma once

#include <stdint.h>

/* 初始化网卡；成功返回 0 */
int e1000_init(void);

/* 轮询收包：有帧就 dump 到控制台 */
void e1000_poll_rx(void);

/* 发送一帧（以太网帧）；成功返回 0 */
int e1000_send_frame(const uint8_t *buf, uint32_t len);

/* netd 轮询任务（挂在调度器上） */
void net_poll_task(void);
