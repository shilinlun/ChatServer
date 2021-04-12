#ifndef PUBLIC_H
#define PUBLIC_H

/*
server和client公共头文件
*/

enum EnMsgType
{
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_ACK, // 登陆响应消息
    REG_MSG,       // 注册消息
    REG_MSG_ACK    // 注册响应消息 

};

#endif