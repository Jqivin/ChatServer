#include <string>
#include "user.hpp"

class UserModel
{
public:
    UserModel();
    ~UserModel();
    // 注册用户
    bool insertUser(User &user);

    // 根据用户号码查询用户信息
    User query(int id);

    // 更新用户在线状态
    bool updateState( User user);

private:
};