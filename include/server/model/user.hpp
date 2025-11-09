#pragma once
/*
定义用户信息
*/
#include <string>

class User
{
public:
    User(std::string name = "", std::string pwd = "",  std::string state = "offline")
        : name_(name), password_(pwd), state_(state)
    {
    }
    ~User() {}

    void setId(int id) { id_ = id; }
    void setName(const std::string &name) { name_ = name; }
    void setPwd(const std::string &pwd) { password_ = pwd; }
    void setState(const std::string &state) { state_ = state; }

    int getId() { return id_; }
    std::string getName() { return name_; }
    std::string getPwd() { return password_; }
    std::string getState() { return state_; }

private:
    int id_;               // 用户id
    std::string name_;     // 用户名
    std::string password_; // 用户密码
    std::string state_;    // 用户在线状态
};