#pragma once

/*
* 聊天服务业务代码
*/
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <mutex>
#include "json.hpp"
#include "user.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"

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
    // 根据消息Id获取对应的业务处理类型
    MsgHandler GetMsgHandler(int msgId);
    // 修改数据库中在线状态
    void clientCloseException(const TcpConnectionPtr &conn);
    // 处理登录业务
    void LoginHandler(const TcpConnectionPtr& conn,const json& js,Timestamp time);
    // 处理注册业务
    void RegisterHandler(const TcpConnectionPtr& conn,const json& js,Timestamp time);
    // 一对一聊天业务
    void OneChatHandler(const TcpConnectionPtr &conn, json &js, Timestamp time);
private:
    ChatService();

    std::unordered_map<int,MsgHandler>          m_msgHandlerMap;        // 业务处理map  msgId->handler
    std::unordered_map<int, TcpConnectionPtr>   m_userConnMap;    // 连接map      用户id->连接
    std::mutex  m_mutexCon;

    UserModel                           m_userModel;             // 用户数据管理
    OfflineMessageModel                 m_offMsgModel;           // 离线消息管理
    
};