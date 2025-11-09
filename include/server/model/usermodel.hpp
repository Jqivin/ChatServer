#include <string>
#include "user.hpp"

class UserModel
{
public:
    UserModel();
    ~UserModel();
    // 注册用户
    bool insertUser(User &user);

private:
};