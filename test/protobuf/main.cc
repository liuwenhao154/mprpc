#include "test.pb.h"
#include <iostream>

int main(){
    // fixbug::LoginResponse rsp;
    // fixbug::ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("Login failed");

    fixbug::GetFriendListResponse rsp;
    fixbug::ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    fixbug::User *user1 = rsp.add_friend_list();
    user1->set_name("Alice");
    user1->set_age(25);
    user1->set_sex(fixbug::User::Woman);

    fixbug::User *user2 = rsp.add_friend_list();
    user2->set_name("Bob");
    user2->set_age(30);
    user2->set_sex(fixbug::User::Man);

    std::cout << rsp.friend_list_size() << std::endl;

    return 0;
}

// int main(){
//     fixbug::LoginRequest req;
//     req.set_name("zhang san");
//     req.set_pwd("123456");

//     std::string send_str;
//     if(req.SerializeToString(&send_str))
//     {
//         std::cout << send_str << std::endl;
//     }

//     fixbug::LoginRequest reqB;
//     if(reqB.ParseFromString(send_str)){
//         std::cout << reqB.name() << std::endl;
//         std::cout << reqB.pwd() << std::endl;
//     }

//     return 0;
// }