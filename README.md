# C2 工具 - 编译和使用说明

## 项目概述

这是一个基于 C++ 的 C2（Command and Control）远程管理工具，采用模块化架构设计。

### 架构组成

- **Server（服务端）**：中转服务器，负责管理所有连接的 Agent 和 Controller
- **Agent（客户端）**：部署在目标机器上的代理程序，支持动态加载功能模块
- **Controller（控制端）**：操作界面，用于向 Agent 发送命令
- **Common（公共库）**：共享的通信协议、加密、网络等功能
- **Modules（功能模块）**：可动态加载的 DLL 模块，实现各种功能

### 功能模块

1. **CommandExec**：执行系统命令
2. **Screenshot**：屏幕截图
3. **Keylogger**：键盘记录
4. **Persistence**：持久化机制

## 编译环境要求

- Visual Studio 2022 或更高版本
- Windows 10 SDK
- C++17 标准支持

## 编译步骤

### 1. 打开解决方案

使用 Visual Studio 打开 `C2Tool.sln` 文件。

### 2. 选择编译配置

- **Debug**：用于开发和调试
- **Release**：用于生产环境，包含优化

### 3. 编译顺序

由于项目之间存在依赖关系，建议按以下顺序编译：

1. 首先编译 **Common** 项目（静态库）
2. 然后编译 **Server**、**Agent**、**Controller**
3. 最后编译所有 **Module** 项目

或者直接选择"生成解决方案"，Visual Studio 会自动处理依赖关系。

### 4. 编译输出

编译完成后，所有可执行文件和 DLL 将输出到：

```
bin\Debug\          或  bin\Release\
├── Server.exe
├── Agent.exe
├── Controller.exe
├── Common.lib
└── modules\
    ├── ModuleCommandExec.dll
    ├── ModuleScreenshot.dll
    ├── ModuleKeylogger.dll
    └── ModulePersistence.dll
```

## 使用说明

### 1. 启动服务端

```bash
Server.exe [端口号]
```

默认端口：8888

示例：
```bash
Server.exe 8888
```

服务端命令：
- `list` - 列出所有连接的客户端
- `quit` - 停止服务器

### 2. 启动 Agent

```bash
Agent.exe [服务器地址] [端口号]
```

示例：
```bash
Agent.exe 127.0.0.1 8888
```

Agent 会自动连接到服务器并注册。

### 3. 启动控制端

```bash
Controller.exe [服务器地址] [端口号]
```

示例：
```bash
Controller.exe 127.0.0.1 8888
```

控制端命令：
- `list` - 列出所有连接的客户端
- `cmd <client_id> <命令>` - 向指定客户端发送命令
- `quit` - 退出控制端

### 4. 模块加载

Agent 支持动态加载功能模块。在 Agent 代码中使用：

```cpp
agent.LoadModule("modules\\ModuleCommandExec.dll", "CommandExec");
```

或者通过网络传输模块文件并动态加载。

## 模块开发指南

### 创建新模块

1. 创建新的 DLL 项目
2. 包含 `ModuleInterface.h` 头文件
3. 实现 `IModule` 接口
4. 使用 `EXPORT_MODULE` 宏导出模块

示例：

```cpp
#include "ModuleInterface.h"

class MyModule : public IModule {
public:
    ModuleInfo GetInfo() const override {
        ModuleInfo info;
        info.name = "MyModule";
        info.version = "1.0.0";
        info.description = "My custom module";
        info.type = ModuleType::COMMAND_EXECUTION;
        info.interfaceVersion = MODULE_INTERFACE_VERSION;
        return info;
    }

    bool Initialize() override {
        return true;
    }

    ModuleResult Execute(const std::vector<uint8_t>& params) override {
        ModuleResult result;
        // 实现功能
        result.success = true;
        return result;
    }

    void Cleanup() override {
        // 清理资源
    }
};

EXPORT_MODULE(MyModule)
```

## 通信协议

### 消息格式

所有消息使用自定义二进制协议：

```
[消息头 14 字节]
- magic (4 字节): 0x43324332 ("C2C2")
- length (4 字节): 消息体长度
- type (1 字节): 消息类型
- encrypted (1 字节): 是否加密
- checksum (2 字节): 校验和

[消息体 N 字节]
- 实际数据内容
```

### 消息类型

- `0x01` HEARTBEAT - 心跳
- `0x02` REGISTER - 注册
- `0x03` COMMAND - 命令
- `0x04` RESPONSE - 响应
- `0x10` FILE_UPLOAD - 文件上传
- `0x11` FILE_DOWNLOAD - 文件下载
- `0x20` SYSTEM_INFO - 系统信息
- `0x21` PROCESS_LIST - 进程列表
- `0x30` SCREENSHOT - 屏幕截图
- `0x31` KEYLOG_START - 开始键盘记录
- `0x40` PERSISTENCE_INSTALL - 安装持久化

## 安全注意事项

### 仅用于授权测试

此工具仅用于：
- 企业内部授权的安全测试

### 加密通信

当前实现使用简单的 XOR 加密作为示例。在生产环境中，应该：
- 使用 AES-256 等强加密算法
- 实现密钥交换机制（如 Diffie-Hellman）
- 添加 SSL/TLS 支持

### 权限管理

- Agent 应该以最小权限运行
- 敏感操作需要额外验证
- 记录所有操作日志

## 故障排除

### 编译错误

1. **找不到 Common.lib**
   - 确保先编译 Common 项目
   - 检查输出目录设置

2. **链接错误**
   - 检查项目依赖关系
   - 确认 ws2_32.lib 已添加到链接器

### 运行时错误

1. **无法连接到服务器**
   - 检查服务器是否已启动
   - 确认防火墙设置
   - 验证 IP 地址和端口

2. **模块加载失败**
   - 确认 DLL 文件存在
   - 检查 DLL 路径是否正确
   - 验证模块接口版本

## 项目结构

```
C2Tool/
├── C2Tool.sln              # Visual Studio 解决方案
├── Common/                 # 公共库
│   ├── Protocol.h/cpp      # 通信协议
│   ├── Crypto.h/cpp        # 加密模块
│   ├── Network.h/cpp       # 网络通信
│   ├── Utils.h/cpp         # 工具函数
│   ├── ModuleInterface.h   # 模块接口
│   └── ModuleLoader.h/cpp  # 模块加载器
├── Server/                 # 服务端
│   ├── Server.h/cpp        # 服务器主类
│   ├── ClientSession.h/cpp # 客户端会话
│   └── main.cpp            # 入口点
├── Agent/                  # 客户端
│   ├── Agent.h/cpp         # Agent 主类
│   └── main.cpp            # 入口点
├── Controller/             # 控制端
│   ├── Controller.h/cpp    # 控制器主类
│   └── main.cpp            # 入口点
└── Modules/                # 功能模块
    ├── CommandExec/        # 命令执行模块
    ├── Screenshot/         # 屏幕截图模块
    ├── Keylogger/          # 键盘记录模块
    └── Persistence/        # 持久化模块
```

## 许可和免责声明

本工具仅供授权的安全测试使用。未经授权使用本工具进行任何非法活动是严格禁止的。使用者需自行承担所有法律责任。
