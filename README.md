# RemoteControl

局域网远程控制工具，支持屏幕画面实时查看、鼠标键盘远程操作。

## 功能

- **屏幕监控** — 实时查看被控端屏幕画面
- **鼠标控制** — 支持移动、单击、双击、右键等全部鼠标操作（共13种动作）
- **键盘控制** — 支持按键按下/弹起
- **配置文件** — 通过 `config.ini` 配置 IP 和端口，无需重新编译

## 项目结构

```
RemoteControl/
├── Common/                    # 公共模块（静态库）
│   ├── Packet.h / .cpp       # 协议定义：封包/解包/结构体
│   └── Config.h / .cpp       # 配置文件读取
├── RemoteControl/             # 被控端（服务端）
│   └── RemoteControl.cpp     # main() 入口，屏幕截图/鼠标键盘模拟
├── Client/Client/             # 控制端（客户端）
│   └── Client.cpp            # WinMain 入口，窗口显示/输入转发
├── config.ini                 # 配置文件（放在 exe 同级目录）
└── RemoteControl.slnx         # 解决方案文件
```

## 编译方法

1. 使用 Visual Studio 打开 `RemoteControl.slnx`
2. 选择 `x64 | Release` 配置
3. 生成解决方案（Ctrl+Shift+B）

> 工具集要求：v143+，Windows SDK 10.0+

## 使用方法

### 1. 配置
下载Release并解压，打开同级的`config.ini` （配置文件需放在 exe 同级目录）：
配置IP,端口，显示窗口。

```ini
[Network]
IP=192.168.32.129
Port=23333
[Screen]
Width=1920
Height=1080
```

### 2. 启动被控端

双击 `RemoteControl.exe`，等待控制端连接。

### 3. 启动控制端

双击 `Client.exe`，输入窗口将显示被控端屏幕画面，可在窗口中进行鼠标键盘操作。

## 通信协议

- **传输层**：TCP
- **应用层协议**：Magic(4B) + Cmd(4B) + BodyLen(4B) + Body(N B)
- Magic: `0x55AA77CC`
- 支持命令：屏幕截图、鼠标操作、键盘操作

## 公网远程

本工具默认适用于 **局域网** 环境。如需公网远程，建议配合内网穿透工具（如 [frp](https://github.com/fatedier/frp)）使用。

## 开发环境

- Visual Studio 2026
- Windows SDK 10.0
- C++20

## 许可证

MIT License
