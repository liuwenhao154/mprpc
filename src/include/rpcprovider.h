#pragma once
#include "google/protobuf/service.h"
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include "mprpcapplication.h"
#include <string>
#include <google/protobuf/descriptor.h>
#include <unordered_map>
#include "logger.h"

class RpcProvider
{
public:
    void NotifyService(google::protobuf::Service *service);

    void Run();
private:
    muduo::net::EventLoop m_eventLoop;

    struct ServiceInfo
    {
        google::protobuf::Service *m_service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> method_map;
    };
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp time);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, 
                        const google::protobuf::Message* response);
};
