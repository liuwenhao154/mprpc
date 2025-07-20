#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"

int main(int argc, char **argv)
{
    // Initialize the MprpcApplication
    MprpcApplication::Init(argc, argv);
    
    // Create a stub for the FriendService
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    
    // Prepare the request
    fixbug::GetFriendListRequest request;
    request.set_userid(12345); // Example user ID

    // Prepare the response
    fixbug::GetFriendListResponse response;
    
    // Create a controller and closure (not used in this example)
    MprpcController controller;
    google::protobuf::Closure *done = nullptr; // Use a real closure in production

    // Call the GetFriendList method
    stub.GetFriendList(&controller, &request, &response, done);

    if(controller.Failed()) {
        std::cerr << "RPC failed: " << controller.ErrorText() << std::endl;
        return -1;
    }
    // Process the response
    if (response.result().errcode() == 0) {
        std::cout << "Friends list retrieved successfully:" << std::endl;
        for (const auto& friend_name : response.friends()) {
            std::cout << " - " << friend_name << std::endl;
        }
    } else {
        std::cout << "Failed to retrieve friends list: " << response.result().errmsg() << std::endl;
    }

    return 0;
}