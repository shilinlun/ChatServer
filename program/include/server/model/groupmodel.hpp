#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"

#include <string>
#include <vector>

using namespace std;

class GroupModel
{

public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群组信息,因为一个用户可能存在于多个群组中
    vector<Group> queryGroups(int userid);
    // 根据指定的groupid查询群组用户id列表，除了userid本省，主要用与群聊服务
    vector<int> queryGroupUsers(int userid, int groupid);

private:
};

#endif