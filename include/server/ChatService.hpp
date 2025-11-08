#pragma once

/*
* 聊天服务业务代码
*/
#include <functional>
#include <muduo/net/TcpConnection.h>
#include "json.hpp"

using namespace muduo;
using namespace muduo::net;
using namespace nlohmann;

// 业务处理类型
using MsgHandler = std::function<void(const TcpConnectionPtr&,json&,Timestamp)>;

class ChatService
{
public:
    // 获取单例聊天服务业务类 对象
    static ChatService* instance();
    // 处理登录业务
    void LoginHandler(const TcpConnectionPtr& conn,const json& js,Timestamp time);
    // 处理注册业务
    void RegisterHandler(const TcpConnectionPtr& conn,const json& js,Timestamp time);
    // 根据消息Id获取对应的业务处理类型
    MsgHandler GetMsgHandler(int msgId);

private:
    ChatService();

    std::unordered_map<int,MsgHandler>  m_msgHandlerMap;        // 业务处理map  msgId->handler
};