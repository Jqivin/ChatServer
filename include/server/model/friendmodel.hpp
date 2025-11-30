#pragma once

/*
好友信息于数据库的交互类
*/
#include <vector>
#include "user.hpp"

class FriendModel
{
public:
    // 插入好友信息
    void insert(int userid,int friendid);
    // 获取好友列表
    std::vector<User> queryfriend(int userid);


};