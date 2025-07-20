#pragma once

#include <semaphore.h>
#include <string>
#include <zookeeper/zookeeper.h>

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    void Start();
    void Creat(const char* path, const char* data, int data_len, int states = 0);
    std::string GetData(const char* path);
private:
    zhandle_t *m_zhandle;
};