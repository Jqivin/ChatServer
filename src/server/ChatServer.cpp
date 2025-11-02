#include "ChatServer.hpp"
#include "json.hpp"
#include <functional>

using namespace std::placeholders;

ChatServer::ChatServer(EventLoop *loop, InetAddress addr, const std::string &strName)
    : loop_(loop), server_(loop, addr, strName)
{
    // 设置连接回调
    server_.setConnectionCallback(std::bind(&ChatServer::OnConnectionCallback, this, _1));
    // 设置消息回调
    server_.setMessageCallback(std::bind(&ChatServer::OnMessageCallback, this, _1, _2, _3));
    // 设置线程数量
    server_.setThreadNum(4);
}
ChatServer::~ChatServer()
{
}

// 启动服务
void ChatServer::Start()
{
    server_.start();
}

// 连接回调函数
void ChatServer::OnConnectionCallback(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}
// 消息回调函数
void ChatServer::OnMessageCallback(const TcpConnectionPtr &conn,
                                   Buffer *buff,
                                   Timestamp time)
{

}
