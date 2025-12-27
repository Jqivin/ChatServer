#pragma once

#include <thread>
#include <hiredis/hiredis.h>
#include <functional>

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis-server
    bool connect();
    // 向redis指定的通道发布消息
    bool publish(int channel, std::string message);
    // 向redis指定的通道subscirbe订阅消息
    bool subscribe(int channel);
    // 向redis指定的通道unsubscirbe取消订阅消息
    bool unsubscribe(int channel);
    // 接受订阅通道的消息（在独立的线程中）
    bool obsever_channel_message();
    // 初始化向业务层上报通道的回调对象
    void init_notify_handler(std::function<void(int, std::string)> fn);

private:
    // hiredis同步上下文对象，负责publish消息
    redisContext *_publish_context;

    // hiredis同步上下文对象，负责subscribe消息
    redisContext *_subcribe_context;

    // 回调操作，收到订阅的消息，给service层上报
    std::function<void(int, std::string)> _notify_message_handler;
};