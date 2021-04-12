#include "chatserver.hpp"
#include "chatsevice.hpp"

#include <iostream>
#include <signal.h>

using namespace std;

// 处理服务器ctrl+c结束后，处理user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

// 网络模块，业务模块，数据库模块三者分开
int main()
{
    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);

    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}