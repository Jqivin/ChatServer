#include "usermodel.hpp"
#include "db.h"
#include <string>

UserModel::UserModel()
{
}

UserModel::~UserModel()
{
}

// 注册用户
bool UserModel::insertUser(User &user)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into user(name,password,state) values('%s','%s','%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        // 更新数据库连接
        if (mysql.update(strSql))
        {
            int id = mysql_insert_id(mysql.getConnection());
            user.setId(id);
            if (id > 0)
            {
                return true;
            }
        }
    }
    return false;
}

// 根据用户号码查询用户信息
User UserModel::query(int id)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select * from user where id = %d", id);

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        // 更新数据库连接
        MYSQL_RES *res = mysql.query(strSql);

        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

bool UserModel::updateState(User user)
{
   // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;
    if (mysql.connectdb())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}