#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo::net;
using namespace muduo;



class ChatServer
{

public:
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);
    void start();

private:
    TcpServer _server;
    EventLoop *_loop;

    // 回调连接相关的事件
    void onConnection(const TcpConnectionPtr &conn);
    // 回调读写事件
    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time);
};

#endif