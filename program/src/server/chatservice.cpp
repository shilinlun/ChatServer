#include "chatsevice.hpp"
#include "public.hpp"

#include <muduo/base/Logging.h>
#include <vector>
#include <string>
using namespace muduo;
using namespace std;

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
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
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
            // 在这里加{}主要就是设置作用域，lock_guard<mutex> lock(_connMutex)中lock_guard会在构造函数加锁，
            // 析构函数释放锁，同时由于只需要insert需要锁，所以加一个作用域，这样避免给不必要的地方加锁，浪费资源
            {
                lock_guard<mutex> lock(_connMutex);
                // 记录用户连接信息
                _userConnMap.insert({id, conn});
            }

            //登陆成功，然后更新用户登陆信息
            user.setState("online");
            _usermodel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 0; //0表示成功
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询该用户是否有离线信息
            vector<string> vec = _offlinemodel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                // 再删除离线消息
                _offlinemodel.remove(id);
            }

            // 查询该用户的好友信息 并返回
            vector<User> uservec = _friendmodel.query(id);
            if (!uservec.empty())
            {
                vector<string> vec2;
                for (auto &i : uservec)
                {
                    json js;
                    js["id"] = i.getId();
                    js["name"] = i.getName();
                    js["state"] = i.getState();
                    vec2.emplace_back(js.dump());
                }
                response["friends"] = vec2;
            }

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

// 处理客户端异常断开
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);

        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                // 找到该conn对应的id，把该键值对从_userConnMap删除
                _userConnMap.erase(it);
                break;
            }
        }
    }
    if (user.getId() != -1)
    {
        // 更新用户的状态信息
        user.setState("offline");
        _usermodel.updateState(user);
    }
}

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    // 对方id
    int toid = js["to"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid在线，转发消息
            it->second->send(js.dump());
            return;
        }
    }

    // 不在线，存储离线消息
    _offlinemodel.insert(toid, js.dump());
}

// 服务器ctrl+c等异常发生，业务重置
void ChatService::reset()
{
    // 把online状态的用户，重置为offline
    _usermodel.resetState();
}

// 添加好友请求 msgid id friendid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    _friendmodel.insert(userid, friendid);
}