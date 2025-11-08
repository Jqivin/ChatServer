#include "db.h"
#include <muduo/base/Logging.h>

using namespace muduo;

const std::string strHost = "127.0.0.1";
const std::string strUser = "root";
const std::string strPassword = "123456";
const std::string strDbName = "chat";

// 初始化数据库连接
MySQL::MySQL()
{
    conn_ = mysql_init(NULL);
    if (conn_ == NULL)
    {
        LOG_ERROR << "msql init error!";
    }
}
// 销毁数据库连接
MySQL::~MySQL()
{
    if (conn_)
    {
        mysql_close(conn_);
    }
}
// 连接数据库
bool MySQL::connectdb()
{
    MYSQL *p = mysql_real_connect(conn_, strHost.c_str(), strUser.c_str(), strPassword.c_str(), strDbName.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(conn_, "set names gbk");
        LOG_INFO << "connect mysql success!";
    }
    else
    {
        LOG_INFO << "connect mysql fail!";
    }

    return p;
}
// 更新
bool MySQL::update(const std::string &strsql)
{
    if (mysql_query(conn_, strsql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << strsql << "update failed!";
        return false;
    }

    return true;
}
// 查询
MYSQL_RES *MySQL::query(const std::string &strsql)
{
    if (mysql_query(conn_, strsql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << strsql << "select failed!";
        return nullptr;
    }

    return mysql_use_result(conn_);
}
// 获取连接
MYSQL *MySQL::getConnection()
{
    return conn_;
}