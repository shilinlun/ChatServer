#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

// 消息id对应的事件回调
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// 聊天服务器业务类,单例模式
class ChatService
{

public:
    // 获取单例
    static ChatService *instance();

    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 添加好友请求
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    // 处理客户端异常断开
    void clientCloseException(const TcpConnectionPtr &conn);

    // 服务器ctrl+c等异常发生，业务重置
    void reset();

private:
    ChatService();
    // 存储消息id和业务处理的方法
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 数据操作类对象
    UserModel _usermodel;
    OfflineMsgModel _offlinemodel;
    FriendModel _friendmodel;

    // 存储在线用户的连接，因为比如用户1向用户2发送请求，这时候需要服务器来做中间转化
    // 所以服务器需要知道用户2是否在线
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 定义互斥锁，保证_userConnMap线程安全，因为每个user都会去操作这个
    mutex _connMutex;
};

#endif