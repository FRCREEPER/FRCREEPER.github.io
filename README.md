# Causenix OS · 下载中心

> 一个能联网的自制 x86_64 操作系统 —— 内核 + LIFS/FAT32 文件系统 + 图形桌面 + e1000 网络栈（V3 UDP 传参执行）

本站是 **Causenix OS 的官方下载站**，提供：

- **内核产物**：`myos.elf`（Limine 引导）
- **用户程序**：desktop / hello / test / format / diskshome 的 ELF
- **内核源码快照**：网络栈相关（e1000 / netcmd / shell / main）

## 下载说明

所有二进制产物以 **base64 文本**（`.b64`）提供，解码即可得到原始 ELF：

```bash
base64 -d myos.elf.b64 > myos.elf
base64 -d desktop.elf.b64 > desktop.elf
```

> 采用 base64 的原因：与 Causenix 未来经 HTTP 拉取后的解析格式对齐（HTTP 响应即文本/字节流，内核端解码后落盘）。

## 网络栈里程碑

| 版本 | 能力 | 状态 |
| --- | --- | --- |
| V1 | e1000 收包 + dump（RX ring，timer 轮询） | ✅ |
| V2 | ARP reply（TX ring + 自动应答，slirp 解析 VM 地址） | ✅ |
| V3 | UDP 传参执行（UDP 7001 → shell 内置命令，host 远程指挥内核） | ✅ |

## 快速开始（QEMU）

```bash
# 1. 解码内核与程序
base64 -d myos.elf.b64 > myos.elf

# 2. 启动（boot.img 已含 Limine + /boot/myos.elf + 用户程序）
qemu-system-x86_64 -machine q35 -m 512M \
  -drive file=boot.img,format=raw \
  -netdev user,id=n1,hostfwd=tcp::7000-:7000,hostfwd=udp::7001-:7001 \
  -device e1000,netdev=n1

# 3. host 远程执行内核命令（V3 验证）
python3 -c 'import socket;s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);s.sendto(b"echo V3-FROM-NET",("127.0.0.1",7001))'
```

## 目录

```
├── index.html          下载中心页面
├── downloads/          可下载产物（base64）
│   ├── myos.elf.b64
│   ├── desktop.elf.b64
│   ├── hello.elf.b64
│   ├── test.elf.b64
│   ├── format.elf.b64
│   └── diskshome.elf.b64
└── src/                内核源码快照（网络栈相关）
```

© Causenix OS · [GitHub @FRCREEPER](https://github.com/FRCREEPER)
