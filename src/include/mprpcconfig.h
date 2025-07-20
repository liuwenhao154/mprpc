#pragma once

#include <unordered_map>
#include <string>

class MprpcConfig
{
public:
    void LoadConfigfile(const char *config_file);
    std::string Load(const std::string &key);
    void Trim(std::string &src_buf);
private:
    std::unordered_map<std::string,std::string> m_configMap;
};