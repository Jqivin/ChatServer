#pragma once

/*
* 用于数据库连接的管理以及数据库相关的操作
*/
#include <mysql/mysql.h>
#include <string>

class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
    // 销毁数据库连接
    ~MySQL();
    // 连接数据库
    bool connectdb();
    // 更新
    bool update(const std::string& strsql);
    // 查询
    MYSQL_RES *query(const std::string& strsql);
    // 获取连接
    MYSQL *getConnection();

private:
    MYSQL *conn_; // mysql连接
};