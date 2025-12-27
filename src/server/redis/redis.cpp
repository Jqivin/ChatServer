#include "redis.hpp"
#include <iostream>

Redis::Redis()
    : _publish_context(nullptr), _subcribe_context(nullptr)
{
}
Redis::~Redis()
{
    if(_publish_context)
    {
        redisFree(_publish_context);
        _publish_context = nullptr;
    }
    if(_subcribe_context)
    {
        redisFree(_subcribe_context);
        _subcribe_context = nullptr;
    }
}
bool Redis::connect()
{
    // 发布消息上下文
    _publish_context = redisConnect("127.0.0.1",6379);
    if(_publish_context)
    {
        std::cout << "redis connect failed." << std::endl;
        return false;
    }
    // 订阅消息上下文
    _subcribe_context =  redisConnect("127.0.0.1",6379);
    if(_subcribe_context)
    {
         std::cout << "redis connect failed." << std::endl;
        return false;
    }

    // 在单独的线程中监听通道上的事件，有消息给上层业务上报
    std::thread t(
        [&](){
            obsever_channel_message();
        }
    );

}
// 向redis指定的通道发布消息
bool Redis::publish(int channel, std::string message)
{
    redisReply* reply = (redisReply*)redisCommand(_publish_context,"PUBLISH %d %s",channel,message.c_str());
    if(reply == nullptr)
    {
        std::cerr << "publish command failed!" << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}
// 向redis指定的通道subscirbe订阅消息
bool Redis::subscribe(int channel)
{
}
// 向redis指定的通道unsubscirbe取消订阅消息
bool Redis::unsubscribe(int channel)
{
}
// 接受订阅通道的消息（在独立的线程中）
bool Redis::obsever_channel_message()
{
}
// 初始化向业务层上报通道的回调对象
void Redis::init_notify_handler(std::function<void(int, std::string)> fn)
{
}