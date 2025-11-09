#pragma once

enum EnMsgType
{
    LOGIN_MSG = 1, // 登录消息
    REG_MSG, // 注册消息
    REG_MSG_ACK, // 注册响应消息
};

enum EnErrorType
{
    SUCCESS = 10000, // 成功
    REG_FAILED, // 注册失败
};