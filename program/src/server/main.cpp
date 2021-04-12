#include "chatserver.hpp"

#include <iostream>

using namespace std;

// 网络模块，业务模块，数据库模块三者分开

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);

    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}