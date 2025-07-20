#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "mprpcapplication.h"
#include "mprpccontroller.h"
#include "logger.h"
#include "zookeeperutil.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* service_desc = method->service();
    std::string service_name = service_desc->name();
    std::string method_name = method->name();

    // get parameter serialization size
    u_int32_t arg_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        arg_size = args_str.size();
    } 
    else {
        controller->SetFailed("request serialize error!");
        return;
    }

    // construct rpc header
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_arg_size(arg_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpc_header.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else {
        controller->SetFailed("rpc_header serialize error!");
        return;
    }
    
    // construct the final message to send
    std::string send_buf;
    send_buf.append(reinterpret_cast<const char*>(&header_size), sizeof(header_size));
    send_buf.append(rpc_header_str);
    send_buf.append(args_str);

    // send the message to the server
    // Assuming you have a method to send data over the network
    // sendDataToServer(send_buf);
    //std::cout << "Sending RPC request: " << send_buf << std::endl;
    LOG_INFO("header_size :%u",header_size);
    LOG_INFO("rpc_header_str: %s", rpc_header_str.c_str());
    LOG_INFO("args_str: %s", args_str.c_str());

    // Here you would typically call a method to actually send the data over the network
    // For example, using a socket:
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0) {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Failed to create socket: %s", strerror(errno));
        controller->SetFailed(err_msg);
        return;
    }

    // read config to get server address and port
    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // int port = std::stoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport"));

    ZkClient zkCli;
    zkCli.Start(); // Start the ZooKeeper client
    // /UserService/Login
    std::string method_path = "/" + service_name + "/" + method_name;
    // 127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data.empty()) 
    {
        controller->SetFailed("Failed to get service address from ZooKeeper.");
        close(clientfd);
        return;
    }
    int idx = host_data.find(":");
    if(idx == -1)
    {
        controller->SetFailed("Invalid service address format.");
        close(clientfd);
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx - 1).c_str());


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Failed to connect to server : %s", strerror(errno));
        controller->SetFailed(err_msg);
        return;
    }

    // Send the data
    if(send(clientfd, send_buf.c_str(), send_buf.size(), 0))
    {
        LOG_INFO("RPC request sent successfully.");
    }
    else {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Failed to send RPC request: %s", strerror(errno));
        controller->SetFailed(err_msg);
        close(clientfd);
        return;
    }

    char recv_buf[1024] = {0};
    int recv_size = 0;
    if((recv_size = recv(clientfd, recv_buf, sizeof(recv_buf), 0)) < 0)
    {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Failed to receive response: %s", strerror(errno));
        controller->SetFailed(err_msg);
        close(clientfd);
        return;
    }

    LOG_INFO("Received response from server.");
    // Deserialize the response
    if(response->ParseFromArray(recv_buf, recv_size))
    {
        LOG_INFO("Response parsed successfully.");
    }
    else {
        LOG_ERROR("Failed to parse response.");
    }

    // Close the socket
    close(clientfd);
}