#pragma once

/*
离线消息的于数据库的交互类
*/

#include <vector>
#include <string>

class OfflineMessageModel
{
public:
    // 插入离线消息
    void insert(int userid,std::string msg);
    // 删除离线消息
    void remove(int userid);
    // 查询离线消息
    std::vector<std::string> query(int userid);
private:
};