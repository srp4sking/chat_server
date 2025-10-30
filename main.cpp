// main.cpp - 简易聊天服务器
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Logging.h>

#include <set>
#include <string>

using namespace muduo;
using namespace muduo::net;

// 存储所有在线用户的连接
std::set<TcpConnectionPtr> g_connections;

// 当收到消息时调用
void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
    std::string msg = buf->retrieveAllAsString();
    
    // 日志：谁发了什么
    LOG_INFO << "From " << conn->name() << ": " << msg;

    // 广播给所有人（包括自己）
    for (const auto& c : g_connections) {
        c->send(msg);
    }
}

// 当连接建立或断开时调用
void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG_INFO << "New user from " << conn->peerAddress().toIpPort();
        g_connections.insert(conn);  // 加入在线列表

        // 发送欢迎语
        conn->send("Welcome to the chat room!\n");
        
    } else {
        LOG_INFO << "User from " << conn->peerAddress().toIpPort() << " left.";
        g_connections.erase(conn);   // 移除连接
    }
}

int main() {
    LOG_INFO << "Chat server is starting on port 9988...";

    EventLoop loop;              // 事件循环
    InetAddress listenAddr(9988); // 监听端口
    TcpServer server(&loop, listenAddr, "ChatServer");

    // 设置回调函数
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();  // 启动服务器
    loop.loop();     // 进入事件循环（阻塞）

    return 0;
}
