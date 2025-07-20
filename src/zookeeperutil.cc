#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <iostream>
#include <semaphore.h>

void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE) {
        sem_t *sem = (sem_t *)zoo_get_context(zh);
        sem_post(sem); // Signal that the connection is established
    }
}

ZkClient::ZkClient():m_zhandle(nullptr)
{
}

ZkClient::~ZkClient()
{
    if (m_zhandle) {
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string conn_str = host + ":" + port;

    m_zhandle = zookeeper_init(
        conn_str.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(nullptr == m_zhandle)
    {
        std::cerr << "Failed to connect to ZooKeeper at " << conn_str << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem); // Wait for the connection to be established
    std::cout << "Connected to ZooKeeper at " << conn_str << std::endl;
}

void ZkClient::Creat(const char* path, const char* data, int data_len, int states)
{
    char path_buffer[256];
    int buffer_len = sizeof(path_buffer);
    int flag;
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (flag == ZNONODE) {
        flag = zoo_create(
            m_zhandle, path, data, data_len,
            &ZOO_OPEN_ACL_UNSAFE, states, path_buffer, buffer_len);
        if (flag == ZOK) {
            std::cout << "Node created at path: " << path << std::endl;
        } else {
            std::cerr << "Failed to create node at path: " << path << ", error: " << zerror(flag) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::GetData(const char* path)
{
    char buffer[256];
    int buffer_len = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &buffer_len, nullptr);
    if (flag == ZOK) {
        return std::string(buffer, buffer_len);
    } else {
        std::cerr << "Failed to get data from path: " << path << ", error: " << zerror(flag) << std::endl;
        return "";
    }
}