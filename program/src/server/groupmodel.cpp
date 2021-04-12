#include "groupmodel.hpp"
#include "db.h"

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 主键id是自增的
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values('%d','%d','%s')", groupid, userid, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
// 查询用户所在群组信息,因为一个用户可能存在于多个群组中
vector<Group> GroupModel::queryGroups(int userid)
{
    /*
    1.先根据userid在groupuser表中查询出该用户所属的群组信息
    2.再根据群组信息，查询属于该群组的所有userid，并且和user表进行多表查询，查出用户的详细信息
    */

    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d", userid);
    vector<Group> groupVec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            
        }
    }
}
// 根据指定的groupid查询群组用户id列表，除了userid本省，主要用与群聊服务
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
}