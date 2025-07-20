#include <iostream>
#include <string>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include <vector>
#include "rpcprovider.h"
#include "friend.pb.h"
#include "logger.h"

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendsList(uint32_t &user_id)
    {
        // Simulate fetching friends list from a database or service
        std::cout << "Fetching friends list for user: " << user_id << std::endl;
        return {"friend1", "friend2", "friend3"};
    }

    //rewrite GetFriendList method
    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t user_id = request->userid();
        std::vector<std::string> friends_list = GetFriendsList(user_id);

        for (const auto& friend_name : friends_list) {
            response->add_friends(friend_name);
        }
        fixbug::ResultCode* result = response->mutable_result();
        result->set_errcode(0); // Assuming 0 means success
        result->set_errmsg("");

        done->Run();
        std::cout << "Friend list retrieved successfully for user: " << user_id << std::endl;
    }
};

int main(int argc, char** argv) {

    LOG_INFO("Starting FriendService...\n");
    LOG_ERROR("This is an error message for testing purposes.\n");

    // Initialize the MprpcApplication
    MprpcApplication::Init(argc, argv);

    // Create an instance of RpcProvider
    RpcProvider provider;

    // Register the FriendService
    provider.NotifyService(new FriendService());

    // Start the RPC service
    provider.Run();

    return 0;
}