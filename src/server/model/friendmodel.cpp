
#include "friendmodel.hpp"
#include "db.h"

// 插入好友信息
void FriendModel::insert(int userid, int friendid)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into friend values(%d,%d)", userid,friendid);

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        mysql.update(strSql);
    }
}

// 获取好友列表
std::vector<User> FriendModel::queryfriend(int userid)
{
     char strSql[1024] = {0};
    sprintf(strSql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d",userid);

    std::vector<User> vec;
    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        MYSQL_RES *res = mysql.query(strSql);
        if (res != nullptr)
        {
            // 把userid用户的好友信息放入vec中返回
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
             mysql_free_result(res);
        }
    }
    return vec;
}