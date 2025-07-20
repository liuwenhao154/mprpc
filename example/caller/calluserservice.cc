#include <iostream>
#include <string>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char **argv)
{
    // Initialize the MprpcApplication
    MprpcApplication::Init(argc, argv);
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("test_user");
    request.set_pwd("123456");

    fixbug::LoginResponse response;
    google::protobuf::RpcController *controller = nullptr; // Use a real controller in production
    google::protobuf::Closure *done = nullptr; // Use a real closure in production

    stub.Login(controller, &request, &response, done);

    // rpc call result
    if(0 == response.result().errcode())
    {
        std::cout << "Login successful: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "Login failed: " << response.result().errmsg() << std::endl;
    }

    //Register method
    fixbug::RegisterRequest req;
    req.set_id(666);
    req.set_name("xxx");
    req.set_pwd("abc");

    fixbug::RegisterResponse resp;
    google::protobuf::RpcController *rcontroller = nullptr;
    google::protobuf::Closure *rdone = nullptr;

    stub.Register(rcontroller,&req,&resp,rdone);

    // rpc call result register
    if(0 == resp.result().errcode())
    {
        std::cout << "Register successful: " << resp.success() << std::endl;
    }
    else
    {
        std::cout << "Register failed: " << resp.result().errmsg() << std::endl;
    }

    return 0;
}