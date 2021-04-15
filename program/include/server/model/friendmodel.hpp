#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"

#include <vector>
using namespace std;

// 提供好友信息相关的操作
class FriendModel
{

public:
    // 添加好友
    void insert(int userid, int friendid);

    // 返回用户的好友列表
    vector<User> query(int userid);

private:
};

#endif