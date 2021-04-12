#include "usermodel.hpp"
#include "db.h"
#include <iostream>
using namespace std;

bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name,password,state) values('%s','%s','%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取成功插入后，用户的id，因为id是自增的
            auto id = mysql_insert_id(mysql.getConnection());
            user.setId(id);
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *ans = mysql.query(sql);
        if (ans != nullptr)
        {
            // 查询成功
            MYSQL_ROW row = mysql_fetch_row(ans);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                // 释放指针
                mysql_free_result(ans);
                return user;
            }
        }
    }
    // 失败就直接返回一个user对象，id为-1
    return User();
}

// 更新用户的信息
bool UserModel::updateState(User user)
{
    // 组装sql语句
    char sql[1024] = {0};

    // 一定要用c_str(),否则是乱码
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}