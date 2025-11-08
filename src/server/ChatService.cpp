#include "ChatService.hpp"
#include "Public.hpp"
#include <muduo/base/Logging.h>

#include <unordered_map>

using namespace muduo;
using namespace std::placeholders;

ChatService::ChatService()
{
    m_msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::LoginHandler,this, _1, _2, _3)});
    m_msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::RegisterHandler,this, _1, _2, _3)});
}

ChatService *ChatService::instance()
{
    static ChatService chatService;
    return &chatService;
}

// 处理登录业务
void ChatService::LoginHandler(const TcpConnectionPtr &conn, const json &js, Timestamp time)
{
    LOG_INFO << "do login";
}

// 处理注册业务
void ChatService::RegisterHandler(const TcpConnectionPtr &conn, const json &js, Timestamp time)
{
    LOG_INFO << "do regist";
}

MsgHandler ChatService::GetMsgHandler(int msgId)
{
    auto iter = m_msgHandlerMap.find(msgId);
    if (iter == m_msgHandlerMap.end())
    {
        // 没找到的话，返回一个默认的业务处理函数  空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "Can not find this msg.MsgId is " << msgId;
        };
    }
    else
    {
        return m_msgHandlerMap[msgId];
    }
}