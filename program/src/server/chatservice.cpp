#include "chatsevice.hpp"
#include "public.hpp"

#include <muduo/base/Logging.h>
using namespace muduo;

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = _usermodel.query(id);
    if (user.getId() == id && user.getPwd() == password)
    {
        if (user.getState() == "online")
        {
            // 用户已经登陆，不允许重复登陆
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 2; // 2表示已经登陆
            response["errormsg"] = "已经登陆，不允许重复登陆";
            conn->send(response.dump());
        }
        else
        {
            //登陆成功，然后更新用户登陆信息
            user.setState("online");
            _usermodel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 0; //0表示成功
            response["id"] = user.getId();
            response["name"] = user.getName();
            conn->send(response.dump());
        }
    }
    else
    {
        // 该用户不存在
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["error"] = 1; //1表示不存在
        response["errormsg"] = "用户名或密码不存在";
        conn->send(response.dump());
    }
}
// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPwd(password);
    bool ans = _usermodel.insert(user);
    if (ans)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["error"] = 0; //0表示成功
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["error"] = 1; //1表示失败
        conn->send(response.dump());
    }
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志，比如msgid没有对应的handler
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认的处理器，用lambda来操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp) {
            LOG_ERROR << "msgid:" << msgid << " can not find handler";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}