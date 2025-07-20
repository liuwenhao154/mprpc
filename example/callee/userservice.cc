#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class UserService : public fixbug::UserServiceRpc
{
    public:
        bool Login(std::string name, std::string pwd)
        {
            std::cout << "doing local service: Login" << std::endl;
            std::cout << "name:" << name << " pwd:" << pwd << std::endl;
            return true;
        }

        bool Register(uint32_t id,std::string name,std::string pwd)
        {
            std::cout << "doing local service: Register" << std::endl;
            std::cout << "id: " << id << "name: " << name << "pwd: " << pwd << std::endl;
            return true;
        }

        void Login(::google::protobuf::RpcController* controller,
                            const ::fixbug::LoginRequest* request,
                            ::fixbug::LoginResponse* response,
                            ::google::protobuf::Closure* done)
        {
            std::string name = request->name();
            std::string pwd = request->pwd();

            bool login_result = Login(name,pwd);

            fixbug::ResultCode *code = response->mutable_result();
            code->set_errcode(1);
            code->set_errmsg("Login error!");
            response->set_success(login_result);

            done->Run();
        }

        void Register(::google::protobuf::RpcController* controller,
                    const ::fixbug::RegisterRequest* request,
                    ::fixbug::RegisterResponse* response,
                    ::google::protobuf::Closure* done)
        {
            uint32_t id = request->id();
            std::string name = request->name();
            std::string pwd = request->pwd();

            bool register_result = Register(id,name,pwd);

            fixbug::ResultCode *code = response->mutable_result();
            code->set_errcode(1);
            code->set_errmsg("Register error!");
            response->set_success(register_result);

            done->Run();
        }
};

int main(int argc,char **argv)
{
    MprpcApplication::Init(argc,argv);

    RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.Run();

    return 0;
}