#include "groupmodel.hpp"
#include "db.h"

// 创建群组
bool GroupModel::CreateGroup(Group &group)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into allgroup(groupname,groupdesc) values('%s','%s')",
            group.getName().c_str(), group.getDesc().c_str());

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        // 更新数据库连接
        if (mysql.update(strSql))
        {
            int id = mysql_insert_id(mysql.getConnection());
            group.setId(id);
            if (id > 0)
            {
                return true;
            }
        }
    }
    return false;
}

// 加入群聊
void GroupModel::AddGroup(int userid, int groupid, std::string role)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into groupuser values(%d,%d,'%s')", groupid,userid,role.c_str());

    // 创建数据库连接
    MySQL mysql;
    if (mysql.connectdb())
    {
        mysql.update(strSql);
    }
}

// 获取用户所在的所有群聊
std::vector<Group> GroupModel::GetGroups(int userid)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */
    char strSql[1024] = {0};
    sprintf(strSql, "select a.groupid,a.groupname,a.groupdesc from allgroup a\
        inner join groupuser b on a.groupid = b.groupid\
        where b.userid=%d", userid);

    std::vector<Group> vecGroups;
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
                Group groupInfo;
                groupInfo.setId(atoi(row[0]));
                groupInfo.setName(row[1]);
                groupInfo.setDesc(row[2]);
                vecGroups.push_back(groupInfo);
            }
             mysql_free_result(res);
        }
    }

    for(Group& group : vecGroups)
    {
        sprintf(strSql, "select a.id,a.name,a.state,b.userrole from user a\
                inner join groupuser b on a.id = b.userid\
                where b.userid=%d", userid);
        
        MYSQL_RES *res = mysql.query(strSql);
        if (res != nullptr)
        {
            // 把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setRole(row[3]);
                
                group.getUsers().push_back(groupUser);
            }
             mysql_free_result(res);
        }
    }

    return vecGroups;
}

// 获取群聊所有成员（除了userid）
std::vector<int> GroupModel::GetGroupUsers(int groupid, int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    std::vector<int> idVec;
    MySQL mysql;
    if (mysql.connectdb())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}
