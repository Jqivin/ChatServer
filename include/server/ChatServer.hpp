#pragma once

/*
* 聊天服务器代码
*/

#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/InetAddress.h"

#include <string>

using namespace muduo;
using namespace muduo::net;

// 聊天服务器
class ChatServer
{
public:
    ChatServer(EventLoop *loop, InetAddress addr, const std::string &strName);
    ~ChatServer();

    // 启动服务
    void Start();
    // 连接回调函数
    void OnConnectionCallback(const TcpConnectionPtr &conn);
    // 消息回调函数
    void OnMessageCallback(const TcpConnectionPtr &conn,
                           Buffer *buff,
                           Timestamp time);

private:
    EventLoop *loop_; // 事件循环
    TcpServer server_; //网络模块
};