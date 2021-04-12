#ifndef GTOUPUSER_H
#define GTOUPUSER_H

#include "user.hpp"
#include <string>

using namespace std;


// 群组用户，多了一个role角色信息，从User类直接继承
class GroupUser : public User
{

public:
    void setRole(string role) { this->role = role; }
    string getRole() { return this->role; }

private:
    string role;
};

#endif
