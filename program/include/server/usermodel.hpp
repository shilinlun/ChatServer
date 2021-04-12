#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// user表的数据操作类
class UserModel
{

public:
    // user表的增加
    bool insert(User &user);
    // 根据用户id查询用户信息
    User query(int id);
    // 更新用户的信息
    bool updateState(User user);

private:
};

#endif