#pragma once

#include "user.hpp"

#define GroupUserRole_Creator "creator"
#define GroupUserRole_Manager "manager"
#define GroupUserRole_Normal "normal"

// 群组用户，多了一个role角色信息，从User类直接继承，复用User的其它信息
class GroupUser : public User
{
public:
    // 设置群聊角色
    void setRole(const std::string &strRole) { m_strRole = strRole; }
    std::string getRole() { return m_strRole; }

private:
    std::string m_strRole; // 角色
};