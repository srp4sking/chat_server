# chat_server

基于 muduo 的高性能 C++ 聊天服务器模板。该项目以 muduo 网络库为核心，提供实时连接管理、房间/频道、用户认证（可扩展）以及基础消息广播与私聊示例。此 README 旨在说明构建、运行、配置和贡献流程。

---

## 目录

- 项目简介
- 特性
- 依赖
- 构建（cmake）
- 运行与配置
- 部署建议（systemd / Docker）
- 协议与使用示例
- 日志与调试
- 测试
- 贡献指南
- 许可证

---

## 项目简介

chat_server 是一个使用 muduo(https://github.com/chenshuo/muduo) 构建的聊天服务器示例/模板。目标是提供一个清晰、可扩展的代码基础，方便在此基础上实现产品级别的实时通信功能。

## 特性

- 基于 epoll 的高性能网络 I/O（muduo）
- 多连接并发处理
- 简单的房间/频道管理
- 消息广播与私聊示例
- 可扩展的用户鉴权钩子（可集成 JWT / 自定义协议）
- 支持将消息持久化到外部存储（需自行集成）

## 依赖

建议依赖（根据项目实际情况调整）：

- C++17 或更高
- cmake >= 3.10
- muduo（推荐源码编译后安装或作为子模块）
- protobuf (可选，用于消息序列化)
- OpenSSL（可选，用于 TLS）
- libgflags / glog（可选，用于日志与标志）

在 Debian/Ubuntu 上示例安装：

```bash
sudo apt update
sudo apt install -y build-essential cmake libssl-dev libprotobuf-dev protobuf-compiler 
# 可选: glog, gflags
sudo apt install -y libglog-dev libgflags-dev
```

如果将 muduo 作为子模块或手动安装，请参考 muduo 的 README 进行编译与安装。

## 构建（cmake）

项目使用 CMake 构建。以下为常见构建流程（项目根目录）：

```bash
# 使用独立构建目录
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release 
# 如果 muduo 安装在非标准路径，可传入 CMAKE_PREFIX_PATH 或 MUDUO_ROOT
# cmake .. -DMUDUO_ROOT=/path/to/muduo -DCMAKE_PREFIX_PATH=/opt/muduo
make -j$(nproc)
```

构建成功后，二进制一般位于 build/bin/ 或 build/ 目录下（取决于 CMakeLists.txt）。

## 运行与配置

示例启动命令：

```bash
# 假设生成二进制为 bin/chat_server
./bin/chat_server --config ../conf/chat_server.conf
```

示例配置文件（conf/chat_server.conf）示例：

```ini
# 端口与网卡
listen_ip = 0.0.0.0
listen_port = 8888

# 日志
log_level = INFO
log_file = /var/log/chat_server/chat_server.log

# 可选：认证/数据库配置占位
db_dsn = postgres://user:pass@127.0.0.1:5432/chatdb
redis_url = redis://127.0.0.1:6379
jwt_secret = your_jwt_secret
```

注意：项目可能使用不同的配置格式（ini / json / yaml），请依据仓库实现调整。

## 协议与使用示例

本项目使用基于 TCP 的自定义消息协议（或二进制/文本 JSON）通过 muduo 传输。下面是一个基于 JSON 的示例协议（仅示意，具体以代码实现为准）：

客户端发送加入房间请求：

```json
{
  "type": "join",
  "room": "general",
  "from": "alice"
}
```

发送消息：

```json
{
  "type": "message",
  "room": "general",
  "from": "alice",
  "text": "hello world"
}
```

服务器广播（示例）：

```json
{
  "type": "message",
  "room": "general",
  "from": "alice",
  "text": "hello world",
  "timestamp": 1630000000
}
```

如果仓库使用 protobuf，请把 proto 文件与生成步骤写入对应目录并在构建说明中补充。

## 日志与调试

- 建议集成 glog / spdlog / plog 等日志库，按级别输出日志。
- 使用 gdb / lldb 或 valgrind/asan 在本地进行内存与崩溃排查。
- 在高并发场景下，可使用 perf / flamegraph / pprof 等工具分析瓶颈。

## 测试

- 单元测试：建议使用 gtest 或 Catch2。
- 压力测试：可以用 wrk / tsung / 自定义客户端脚本模拟大量连接与消息。

示例运行测试（如果项目集成 gtest）：

```bash
cd build
ctest --output-on-failure
```

## 部署建议

- systemd 服务示例：

```ini
[Unit]
Description=chat_server
After=network.target

[Service]
Type=simple
User=chat
Group=chat
WorkingDirectory=/opt/chat_server
ExecStart=/opt/chat_server/bin/chat_server --config /etc/chat_server.conf
Restart=on-failure
LimitNOFILE=200000

[Install]
WantedBy=multi-user.target
```

- Dockerfile（示例，可根据需要扩展）：

```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake libssl-dev libprotobuf-dev protobuf-compiler 
# 可选安装其他依赖
WORKDIR /src
COPY . /src
RUN mkdir build && cd build && cmake .. && make -j$(nproc)
CMD ["/src/build/bin/chat_server"]
```

- 建议部署在多实例后面接负载均衡器（如 L4 负载均衡或使用 DNS 轮询），并在应用层使用一致性哈希或会话粘滞策略（如果需要）。

## 贡献指南

欢迎贡献：
1. Fork 仓库
2. 新建分支：git checkout -b feat/your-feature
3. 提交并推送：git push origin feat/your-feature
4. 发起 PR，描述变更与兼容性影响

请在 PR 中包含：构建/运行步骤、测试方法以及必要的性能说明。

## 许可证

本仓库默认使用 MIT 许可证（如需更改请替换为实际许可证）。

---

如果你同意，我将在仓库根目录创建/更新 README.md 并提交到 main 分支。请确认是否需要我在 README 中附带具体的 CMakeLists.txt 路径、二进制名或配置文件路径（如果有特定位置我应使用），否则我将采用通用路径（bin/chat_server、conf/chat_server.conf）。