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