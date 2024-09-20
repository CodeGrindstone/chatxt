//
// Created by 31435 on 2024/9/4.
//

#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H


#include "Singleton.h"
#include "../head.h"
#include "../RedisMgr/RedisMgr.h"

#include <map>
#include <functional>

class HttpConn;
typedef std::function<void(std::shared_ptr<HttpConn>)> HttpHandler;
class LogicSystem:public Singleton<LogicSystem>,
public std::enable_shared_from_this<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool HandleGet(std::string url, std::shared_ptr<HttpConn> conn);
    bool HandlePost(std::string url, std::shared_ptr<HttpConn> conn);
    void RegGet(std::string, HttpHandler);
    void RegPost(std::string, HttpHandler);
private:
    LogicSystem();

private:
    std::map<std::string, HttpHandler> m_PostHandlers;
    std::map<std::string, HttpHandler> m_GetHandlers;
};



#endif //LOGICSYSTEM_H
