#include "offlinemessagemodel.hpp"
#include "db.h"

void OfflineMessageModel::insert(int userid, std::string msg)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into offlinemessage(userid,msg) values(%d,'%s')", userid, msg.c_str());

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        mysql.update(strSql);
    }
}

// 删除离线消息
void OfflineMessageModel::remove(int userid)
{
    char strSql[1024] = {0};
    sprintf(strSql, "delete from offlinemessage where userid = %d", userid);

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        mysql.update(strSql);
    }
}

// 查询离线消息
std::vector<std::string> OfflineMessageModel::query(int userid)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select msg from offlinemessage where userid='%d'", userid);

    std::vector<std::string> vec;
    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        MYSQL_RES *res = mysql.query(strSql);
        if (res != nullptr)
        {
            // 把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
             mysql_free_result(res);
        }
    }
    return vec;
}