//
// Created by 31435 on 2024/9/4.
//

#include "LogicSystem.h"
#include "../HttpConn/HttpConn.h"

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConn> conn)
{
    if(m_GetHandlers.find(url) == m_GetHandlers.end())
    {
        return false;
    }
    m_GetHandlers[url](conn);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    m_GetHandlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConn> connection) {
        beast::ostream(connection->m_response.body()) << "receiv eget_test req";
        int i = 0;
        for(auto& elem:connection->m_get_params)
        {
            i++;
            beast::ostream(connection->m_response.body()) << "param " << i << " " << elem.first << " " << elem.second;
        }
    });

}
