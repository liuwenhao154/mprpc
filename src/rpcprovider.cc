#include "rpcprovider.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"

void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;

    const google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();
    std::string service_name = pServiceDesc->name();
    int methodCnt = pServiceDesc->method_count();

    LOG_INFO("Registering service: %s with %d methods", service_name.c_str(), methodCnt);

    for (int i = 0; i < methodCnt; ++i)
    {
        const google::protobuf::MethodDescriptor *pMethodDesc = pServiceDesc->method(i);
        std::string method_name = pMethodDesc->name();
        service_info.method_map.insert({method_name, pMethodDesc});
        LOG_INFO("Registered method: %s for service: %s", method_name.c_str(), service_name.c_str());
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(
        MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str()
    );
    muduo::net::InetAddress addr(ip, port);
    muduo::net::TcpServer server(&m_eventLoop, addr, "RpcProvider");
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, 
                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4); // Set the number of threads for handling requests

    ZkClient zkCli;
    zkCli.Start(); // Start the ZooKeeper client
    for (auto& sp : m_serviceMap)
    {
        // Create a service path in ZooKeeper
        // If the service already exists, it will not be created again
        std::string service_path = "/" + sp.first;
        zkCli.Creat(service_path.c_str(), nullptr, 0);
        for(auto &mp : sp.second.method_map)
        {
            // reserve the method path in ZooKeeper
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[256] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            zkCli.Creat(method_path.c_str(), method_path_data, sizeof(method_path_data), ZOO_EPHEMERAL);
        }
    }

    LOG_INFO("RpcProvider is starting at %s:%d", ip.c_str(), port);
    // Start the server
    server.start();
    m_eventLoop.loop(); // Start the event loop to listen for incoming connections
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        conn->shutdown(); // Shutdown the connection if it is not connected
    }
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, 
                    const google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
        LOG_INFO("Response sent successfully.");
    }
    else
    {
        LOG_ERROR("Failed to serialize response.");
    }
    conn->shutdown(); // Shutdown the connection after sending the response
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,
                muduo::net::Buffer* buf,
                muduo::Timestamp time)
{
    std::string recv_buf = buf->retrieveAllAsString();

    //read the header
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, sizeof(header_size), 0);

    std::string rpc_header_str = recv_buf.substr(sizeof(header_size), header_size);
    mprpc::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t arg_size;
    if(rpc_header.ParseFromString(rpc_header_str))
    {
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        arg_size = rpc_header.arg_size();
    }
    else
    {
        LOG_ERROR("Failed to parse RpcHeader from string.");
        return;
    }
    // get method parameters
    std::string args_str = recv_buf.substr(sizeof(header_size) + header_size, arg_size);

    //print debug info
    LOG_INFO("header_size: %u", header_size);
    LOG_INFO("rpc_header_str: %s", rpc_header_str.c_str());
    LOG_INFO("service_name: %s", service_name.c_str());
    LOG_INFO("method_name: %s", method_name.c_str());
    LOG_INFO("args_str: %s", args_str.c_str());

    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        LOG_ERROR("Service not found: %s", service_name.c_str());
        return;
    }

    google::protobuf::Service *service = it->second.m_service;

    auto method_it = it->second.method_map.find(method_name);
    if(method_it == it->second.method_map.end())
    {
        LOG_ERROR("Method not found: %s in service: %s", method_name.c_str(), service_name.c_str());
        return;
    }

    const google::protobuf::MethodDescriptor *method = method_it->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        LOG_ERROR("Failed to parse request for method: %s", method_name.c_str());
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    google::protobuf::Closure *done = 
        google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, const google::protobuf::Message*>
                                    (this, &RpcProvider::SendRpcResponse, conn, response);

    service->CallMethod(method, nullptr, request, response, done);
}