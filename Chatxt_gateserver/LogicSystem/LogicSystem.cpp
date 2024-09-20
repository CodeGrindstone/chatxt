//
// Created by 31435 on 2024/9/4.
//

#include "LogicSystem.h"
#include "../HttpConn/HttpConn.h"
#include "../VerifyGrpcClient/VerifyGrpcClient.h"
#include "../RedisMgr/RedisMgr.h"
#include "../MysqlDao/MysqlMgr.h"

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

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConn> conn)
{
    if(m_PostHandlers.find(url) == m_PostHandlers.end())
    {
        return false;
    }
    m_PostHandlers[url](conn);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    m_GetHandlers.insert(std::make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    m_PostHandlers.insert(std::make_pair(url, handler));
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

    RegPost("/get_varifycode", [](std::shared_ptr<HttpConn> connection)
    {
        auto body_str = beast::buffers_to_string(connection->m_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->m_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        }

        auto email = src_root["email"].asString();
        GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
        std::cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->m_response.body()) << jsonstr;
    });

    RegPost("/user_register", [](std::shared_ptr<HttpConn> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->m_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        auto email = src_root["email"].asString();
        auto name = src_root["user"].asString();
        auto pwd = src_root["passwd"].asString();
        auto confirm = src_root["confirm"].asString();

        std::cout << "email is " << email << std::endl;
        std::cout << "user is " << name << std::endl;
        std::cout << "pwd is " << pwd << std::endl;
        std::cout << "confirm is " << confirm << std::endl;

        //先查找redis中email对应的验证码是否合理
        std::string  varify_code;
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(), varify_code);
        if (!b_get_varify) {
            std::cout << " get varify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        if (varify_code != src_root["varifycode"].asString()) {
            std::cout << " varify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        //查找数据库判断用户是否存在
        int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
        if (uid == 0 || uid == -1) {
            std::cout << " user or email exist" << std::endl;
            root["error"] = ErrorCodes::UserExist;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        root["error"] = 0;
        root["uid"] = uid;
        root["email"] = email;
        root ["user"]= name;
        root["passwd"] = pwd;
        root["confirm"] = confirm;
        root["varifycode"] = src_root["varifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->m_response.body()) << jsonstr;
        return true;
    });
}
