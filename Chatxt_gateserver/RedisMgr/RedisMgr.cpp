//
// Created by 31435 on 2024/9/12.
//

#include "RedisMgr.h"
#include <cstring>
#include <boost/bind/bind.hpp>

#include "../ConfigMgr/ConfigMgr.h"

RedisMgr::~RedisMgr()
{
    Close();
}

// bool RedisMgr::Connect(const std::string& host, int port)
// {
//     m_connect = redisConnect(host.c_str(), port);
//     // 检查连接是否成功
//     if (this->m_connect != NULL && this->m_connect->err)
//     {
//         std::cout << "connect error " << this->m_connect->errstr << std::endl;
//         return false;
//     }
//     return true;
// }

bool RedisMgr::Get(const std::string& key, std::string& value)
{
    auto m_connect = m_connpool->GetRedisContext();

    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "GET %s", key.c_str()));
    // 检查命令执行结果，确保 reply 不为空
    if (!reply) {
        std::cerr << "[GET " << key << "] Command failed: No response from Redis." << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    // 检查返回类型是否为字符串
    if (reply->type != REDIS_REPLY_STRING) {
        std::cerr << "[GET " << key << "] Command failed: Incorrect reply type. Expected STRING, got type "
                  << reply->type << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    value = reply->str;
    std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::Set(const std::string &key, const std::string &value){
    auto m_connect = m_connpool->GetRedisContext();
    //执行redis命令行
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect,"SET %s %s", key.c_str(), value.c_str()));
    //如果返回NULL则说明执行失败;
    if (NULL == reply)
    {
        std::cerr << "Command execution failed: [SET " << key << " " << value << "] - No response from Redis." << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    // 检查 Redis 返回的类型和状态是否是期望的 "OK"
    if (reply->type != REDIS_REPLY_STATUS || (strcmp(reply->str, "OK") != 0)) {
        std::cerr << "Command execution failed: [SET " << key << " " << value << "] - Invalid response." << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::Auth(const std::string& password)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "AUTH %s", password.c_str()));
    if(nullptr == reply)
    {
        std::cerr << "Redis command execution failed" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    if(reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "认证失败" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    std::cout << "认证成功" << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "LPUSH %s %s", key.c_str(), value.c_str()));

    if(nullptr == reply)
    {
        std::cerr << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    bool success = (reply->type == REDIS_REPLY_INTEGER && reply->integer > 0);
    if(success)
    {
        std::cout << "[LPUSH " << key << " " << value << "] success ! " << std::endl;
    }
    else
    {
        std::cout << "[LPUSH " << key << " " << value << "] command execution failed." << std::endl;
    }
    m_connpool->ReturnConnRedis(m_connect);
    return success;
}

bool RedisMgr::LPop(const std::string& key, std::string& value)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "LPOP %s ", key.c_str()));
    if(nullptr == reply)
    {
        std::cerr << "Execut command [ LPOP " << key << "  " << value << " ] failure ! " << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    // 检查返回值是否为 NIL，表示列表为空
    if (reply->type == REDIS_REPLY_NIL) {
        std::cerr << "Execut command [ LPOP " << key << " ] failed: list is empty!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    // 提取返回的值
    if (reply->type == REDIS_REPLY_STRING) {
        value = reply->str;
        std::cout << "Execut command [ LPOP " << key << " ] success!" << std::endl;
    } else {
        std::cerr << "Execut command [ LPOP " << key << " ] failed: unexpected reply type!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "RPUSH %s %s", key.c_str(), value.c_str()));
    if(nullptr == reply)
    {
        std::cerr << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    bool success = (reply->type == REDIS_REPLY_INTEGER && reply->integer > 0);
    if(success)
    {
        std::cout << "[RPUSH " << key << " " << value << "] success ! " << std::endl;
    }
    else
    {
        std::cout << "[RPUSH " << key << " " << value << "] command execution failed." << std::endl;
    }
    m_connpool->ReturnConnRedis(m_connect);
    return success;
}

bool RedisMgr::RPop(const std::string& key, std::string& value)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "RPOP %s ", key.c_str()));
    if(nullptr == reply)
    {
        std::cerr << "Execut command [ RPOP " << key << "  " << value << " ] failure ! " << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    // 检查返回值是否为 NIL，表示列表为空
    if (reply->type == REDIS_REPLY_NIL) {
        std::cerr << "Execut command [ LPOP " << key << " ] failed: list is empty!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    // 提取返回的值
    if (reply->type == REDIS_REPLY_STRING) {
        value = reply->str;
        std::cout << "Execut command [ RPOP " << key << " ] success!" << std::endl;
    } else {
        std::cerr << "Execut command [ RPOP " << key << " ] failed: unexpected reply type!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str()));
    if(nullptr == reply)
    {
        std::cerr << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure: Command execution failed!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    if(reply->type != REDIS_REPLY_INTEGER)
    {
        std::cerr << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure: Unexpected reply type!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    auto m_connect = m_connpool->GetRedisContext();
    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    RedisReplyPtr reply((redisReply*)redisCommandArgv(m_connect, 4, argv, argvlen));
    if(nullptr == reply)
    {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    if(reply->type != REDIS_REPLY_NIL)
    {
        std::cerr << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure: Unexpected reply type!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
    auto m_connect = m_connpool->GetRedisContext();
    // 通过 redisCommand 发送 HGET 命令，直接格式化字符串传递参数
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "HGET %s %s", key.c_str(), hkey.c_str()));

    // 检查命令执行结果
    if (reply == nullptr) {
        std::cerr << "Execut command [ HGet " << key << " " << hkey << " ] failure: Command execution failed!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return "";
    }

    if (reply->type == REDIS_REPLY_NIL) {
        std::cerr << "Execut command [ HGet " << key << " " << hkey << " ] failure: Key or field does not exist!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return "";
    }

    // 获取返回的值
    std::string value = reply->str;

    std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success!" << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return value;
}

bool RedisMgr::Del(const std::string& key)
{
    auto m_connect = m_connpool->GetRedisContext();
    // 使用 redisCommand 发送 DEL 命令，并使用 unique_ptr 自动管理 redisReply 对象
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "DEL %s", key.c_str()));

    // 检查命令是否执行成功
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        std::cerr << "Execut command [ Del " << key << " ] failure: Command execution failed or unexpected reply type!" << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }

    // 判断是否有键被删除
    if (reply->integer == 0) {
        std::cout << "Execut command [ Del " << key << " ] warning: No keys were deleted!" << std::endl;
    } else {
        std::cout << "Execut command [ Del " << key << " ] success: " << reply->integer << " keys deleted!" << std::endl;
    }
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
    auto m_connect = m_connpool->GetRedisContext();
    RedisReplyPtr reply((redisReply*)redisCommand(m_connect, "exists %s", key.c_str()));
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        m_connpool->ReturnConnRedis(m_connect);
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    m_connpool->ReturnConnRedis(m_connect);
    return true;
}

void RedisMgr::Close()
{
    m_connpool->Close();
}

RedisMgr::RedisMgr()
{
    auto& gCfgMgr = ConfigMgr::getInstance();
    auto host = gCfgMgr["Redis"]["Host"];
    auto port = gCfgMgr["Redis"]["Port"];
    auto passwd = gCfgMgr["Redis"]["Passwd"];
    m_connpool.reset(new RedisConnPool(6, host, atoi(port.c_str()), passwd));
}