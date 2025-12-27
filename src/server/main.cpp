#include "ChatServer.hpp"
#include "ChatService.hpp"
#include <signal.h>
#include <iostream>

// CTRL+C异常终止服务器处理逻辑，不走系统结束
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
     if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << std::endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 处理ctrl+C异常终止服务器
    signal(SIGINT,resetHandler);
    EventLoop loop;
    InetAddress addr(ip,port);

    ChatServer server(&loop,addr,"ChatServer");
    
    server.Start();
    loop.loop();

    return 0;
}