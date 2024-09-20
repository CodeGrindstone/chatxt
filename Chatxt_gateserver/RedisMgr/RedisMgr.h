//
// Created by 31435 on 2024/9/12.
//

#ifndef REDISMGR_H
#define REDISMGR_H

#include "../LogicSystem/Singleton.h"

#include <memory>
#include <hiredis/hiredis.h>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <condition_variable>
#include <boost/mpl/size.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

class RedisConnPool
{
public:
    RedisConnPool(size_t PoolSize, std::string host, int port, const std::string pwd):
    b_stop(false), m_poolsize(PoolSize), m_host(host), m_port(port)
    {
        for(int i = 0; i < PoolSize; i++)
        {
            redisContext* context = redisConnect(host.c_str(), port);
            if(context == nullptr || context->err != 0)
            {
                if(context != nullptr)
                {
                    redisFree(context);
                }
                continue;
            }
            auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd.c_str());
            if(reply->type == REDIS_REPLY_ERROR)
            {
                std::cout << "认证失败" << std::endl;
                freeReplyObject(reply);
                continue;
            }
            freeReplyObject(reply);
            std::cout << "认证成功" << std::endl;
            m_connque.push(context);
        }
    }
    ~RedisConnPool()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(!m_connque.empty())
        {
            m_connque.pop();
        }
        b_stop = true;
    }
    redisContext* GetRedisContext()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_con.wait(lock, [this]()
        {
            if(b_stop)
                return true;
            return !m_connque.empty();
        });
        if(b_stop)
            return nullptr;
        redisContext* context = m_connque.front();
        m_connque.pop();
        return context;
    }

    void ReturnConnRedis(redisContext* context)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(b_stop)
            return;
        m_connque.push(context);
        m_con.notify_all();
    }

    void Close()
    {
        b_stop = true;
        m_con.notify_all();
    }

private:
    std::atomic<bool> b_stop;
    size_t m_poolsize;
    const std::string m_host;
    const int m_port;
    std::queue<redisContext *> m_connque;
    std::mutex m_mutex;
    std::condition_variable m_con;

};



class RedisMgr : public Singleton<RedisMgr>, public std::enable_shared_from_this<RedisMgr>
{
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    // bool Connect(const std::string& host, int port);
    bool Get(const std::string &key, std::string& value);
    bool Set(const std::string &key, const std::string &value);
    bool Auth(const std::string &password);
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);
    bool Del(const std::string &key);
    bool ExistsKey(const std::string &key);
    void Close();
private:
    RedisMgr();
    // 自定义删除器，用于释放 redisReply 对象
    struct RedisReplyDeleter
    {
        void operator()(redisReply *reply) const
        {
            if (reply)
                freeReplyObject(reply);
        }
    };
    std::shared_ptr<RedisConnPool> m_connpool;
    // 使用智能指针管理redisReply对象
    using RedisReplyPtr = std::unique_ptr<redisReply, RedisReplyDeleter>;
};



#endif //REDISMGR_H
