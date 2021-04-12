#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include "json.hpp"
#include "usermodel.hpp"

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

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);

private:
    ChatService();
    // 存储消息id和业务处理的方法
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 数据操作类对象
    UserModel _usermodel;
};

#endif