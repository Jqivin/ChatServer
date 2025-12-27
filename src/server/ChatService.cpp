#include "ChatService.hpp"
#include "Public.hpp"
#include <muduo/base/Logging.h>
#include "group.hpp"

#include <unordered_map>

using namespace muduo;
using namespace std::placeholders;

ChatService::ChatService()
{
    // 用户基本业务管理相关事件处理回调注册
    m_msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::LoginHandler, this, _1, _2, _3)});
    m_msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::RegisterHandler, this, _1, _2, _3)});
    m_msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::OneChatHandler, this, _1, _2, _3)});
    m_msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::OnAddFriendHandler, this, _1, _2, _3)});
    m_msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::Loginout, this, _1, _2, _3)});
    // 群组业务管理相关事件处理回调注册
    m_msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::CreateGroup, this, _1, _2, _3)});
    m_msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::AddGroup, this, _1, _2, _3)});
    m_msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::GroupChat, this, _1, _2, _3)});
}

ChatService *ChatService::instance()
{
    static ChatService chatService;
    return &chatService;
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

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> locker(m_mutexCon);
        for (auto iter = m_userConnMap.begin(); iter != m_userConnMap.end(); ++iter)
        {
            if (iter->second == conn)
            {
                user.setId(iter->first);
                m_userConnMap.erase(iter);
                break;
            }
        }
    }

    // 修改数据库中的在线状态
    if (user.getId() != -1)
    {
        user.setState("offline");
        m_userModel.updateState(user);
    }

    LOG_INFO << "clientCloseException success";
}

// 处理注册业务
void ChatService::RegisterHandler(const TcpConnectionPtr &conn, const json &js, Timestamp time)
{
    std::string name = js["name"];
    std::string password = js["password"];
   int iRequestId = 0;
    if(js.contains("requestId"))
    {
       iRequestId = js["requestId"].get<int>();
    }
    User user;
    user.setName(name);
    user.setPwd(password);
    bool bRet = m_userModel.insertUser(user);

    json jsResponse;
    jsResponse["msgid"] = REG_MSG;
    jsResponse["requestId"] = iRequestId;
    if (bRet)
    {
        LOG_INFO << "insert usr success.";

        jsResponse["errno"] = SUCCESS;
        jsResponse["id"] = user.getId();
    }
    else
    {
        LOG_ERROR << "insert usr failed.";

        jsResponse["errno"] = REG_FAILED;
        jsResponse["errmsg"] = "insert usr failed";
    }

    conn->send(jsResponse.dump().c_str());
}

// 处理登录业务
void ChatService::LoginHandler(const TcpConnectionPtr &conn, const json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    std::string strPwd = js["password"];
    int iRequestId = 0;
    if(js.contains("requestId"))
    {
       iRequestId = js["requestId"].get<int>();
    }
    

    User user = m_userModel.query(id);
    if (user.getId() == id && user.getPwd() == strPwd)
    {
        // 成功
        if (user.getState() == "online")
        {
            // 已经登录过了
            json res;
            res["msgid"] = LOGIN_MSG_ACK;
            res["errno"] = LOGIN_HAD_ONLINE;
            res["errmsg"] = "this account is using, input another!";
            res["requestId"] = iRequestId;
            conn->send(res.dump());
        }
        else
        {
            // 登录成功，记录用户连接信息
            {
                std::lock_guard<std::mutex> locker(m_mutexCon);
                m_userConnMap.insert({user.getId(), conn});
            }
            user.setState("online");
            m_userModel.updateState(user);

            // 返回客户端信息
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["requestId"] = iRequestId;
            response["errno"] = 10000;
            response["errmsg"] = "success";
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 离线消息
            std::vector<std::string> vecOff = m_offMsgModel.query(id);
            if (!vecOff.empty())
            {
                response["offlinemsg"] = vecOff;
                // 删除数据库
                m_offMsgModel.remove(id);
            }
            // 获取好友消息
            json jsonFriends;
            if (GetFriendInfo(id, jsonFriends))
            {
                response["friend"] = jsonFriends;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        // 失败  用户名或密码错误
        json res;
        res["msgid"] = LOGIN_MSG_ACK;
        res["errno"] = LOGIN_NAME_PWD_ERR;
        res["requestId"] = iRequestId;
        res["errmsg"] = "This account or password error!";
        conn->send(res.dump());
    }
}

// 一对一聊天业务
void ChatService::OneChatHandler(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();

    {
        std::lock_guard<std::mutex> locker(m_mutexCon);
        auto iter = m_userConnMap.find(toid);
        if (iter != m_userConnMap.end())
        {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            iter->second->send(js.dump());
            return;
        }
    }

    LOG_INFO << "add offline message:" << js.dump();
    // toid不在线，存储离线消息
    m_offMsgModel.insert(toid, js.dump());
}

// 添加好友业务
void ChatService::OnAddFriendHandler(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int usrid = js["userid"].get<int>();
    int friendid = js["friendid"].get<int>();

    m_friendModel.insert(usrid, friendid);
}

// 创建群组业务
void ChatService::CreateGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    std::string strGroupname = js["groupname"];
    std::string strGroupdesc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(userid, strGroupname, strGroupdesc);

    bool bRet = m_groupModel.CreateGroup(group);
    if (bRet)
    {
        // 存储群组创建人信息
        m_groupModel.AddGroup(userid, group.getId(), "creator");
    }
    // else
    // {
    //      // 失败  创建群聊失败
    //     json res;
    //     res["msgid"] = LOGIN_MSG_ACK;
    //     res["errno"] = LOGIN_NAME_PWD_ERR;
    //     res["errmsg"] = "CreateGroup failed!";
    //     conn->send(res.dump());
    // }
}
// 添加群聊
void ChatService::AddGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    int groupid = js["groupid"].get<int>();
    m_groupModel.AddGroup(userid, groupid, "normal");
}
// 群组聊天业务
void ChatService::GroupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int groupid = js["groupid"].get<int>();
    // 发送者id
    int userid = js["userid"].get<int>();

    std::vector<int> vecGroupUser = m_groupModel.GetGroupUsers(groupid, userid);
    std::lock_guard<std::mutex> locker(m_mutexCon);
    for (auto user : vecGroupUser)
    {
        auto iter = m_userConnMap.find(user);
        if (iter != m_userConnMap.end())
        {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            iter->second->send(js.dump());
            return;
        }
        else
        {
            // toid不在线，存储离线消息
            m_offMsgModel.insert(user, js.dump());
        }
    }
}
void ChatService::reset()
{
    m_userModel.resetStates();
}

bool ChatService::GetFriendInfo(int userid, json &jsonFriends)
{
    std::vector<User> vecUser = m_friendModel.queryfriend(userid);

    if (vecUser.empty())
    {
        return false;
    }

    for (auto &user : vecUser)
    {
        json jsUserInfo;
        jsUserInfo["userid"] = user.getId();
        jsUserInfo["name"] = user.getName();
        jsUserInfo["state"] = user.getState();

        jsonFriends.push_back(jsUserInfo);
    }

    return true;
}

void ChatService::Loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    LOG_INFO << "userid is loginout " << userid;
    {
        std::lock_guard<std::mutex> lock(m_mutexCon);
        auto it = m_userConnMap.find(userid);
        if (it != m_userConnMap.end())
        {
            m_userConnMap.erase(it);
        }
    }

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    if(!m_userModel.updateState(user))
    {

    }
}