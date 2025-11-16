#pragma once

enum EnMsgType
{
    REG_MSG = 1, // 注册消息
    LOGIN_MSG, // 登录消息
    REG_MSG_ACK, // 注册响应消息
    LOGIN_MSG_ACK, // 登录响应消息
};

enum EnErrorType
{
    SUCCESS = 10000, // 成功
    REG_FAILED, // 注册失败
    LOGIN_HAD_ONLINE, // 已经登录过
    LOGIN_NAME_PWD_ERR, // 用户名或密码错误

};