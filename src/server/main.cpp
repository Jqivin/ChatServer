#include "ChatServer.hpp"
#include "ChatService.hpp"
#include <signal.h>

// CTRL+C异常终止服务器处理逻辑，不走系统结束
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}
int main()
{
    // 处理ctrl+C异常终止服务器
    signal(SIGINT,resetHandler);
    EventLoop loop;
    InetAddress addr("127.0.0.1",6000);

    ChatServer server(&loop,addr,"ChatServer");
    
    server.Start();
    loop.loop();

    return 0;
}