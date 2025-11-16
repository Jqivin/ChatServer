#include "ChatServer.hpp"
#include "json.hpp"
#include "ChatService.hpp"
#include <muduo/base/Logging.h>

#include <functional>

using namespace std::placeholders;
using namespace nlohmann;

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
        // 修改数据库中在线状态
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}
// 消息回调函数
void ChatServer::OnMessageCallback(const TcpConnectionPtr &conn,
                                   Buffer *buff,
                                   Timestamp time)
{
    std::string strMessage = buff->retrieveAllAsString();
    if(strMessage.empty())
    {
        LOG_ERROR << "request is empty.";
        return;
    }
    // 序列化
    json jsMes = json::parse(strMessage);
    auto msgHandler = ChatService::instance()->GetMsgHandler(jsMes["msgid"].get<int>());
    msgHandler(conn,jsMes,time);

}
