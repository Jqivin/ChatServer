#pragma once

/*
群聊信息与数据库的交互类
*/
#include <vector>
#include <string>

#include "group.hpp"
#include "groupuser.hpp"

class GroupModel
{
public:
    // 创建群组
    bool CreateGroup(Group &group);
    // 加入群聊
    void AddGroup(int userid, int groupid, std::string role);
    // 获取用户所在的所有群聊信息
    std::vector<Group> GetGroups(int userid);
    // 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
    std::vector<int> GetGroupUsers(int groupid,int userid);

};