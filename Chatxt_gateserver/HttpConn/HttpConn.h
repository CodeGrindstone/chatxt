//
// Created by 31435 on 2024/9/4.
//

#ifndef HTTPCONN_H
#define HTTPCONN_H

#include <unordered_map>

#include "../head.h"
#include "../LogicSystem/LogicSystem.h"
class HttpConn : public std::enable_shared_from_this<HttpConn>
{
    friend class LogicSystem;
public:
    HttpConn(tcp::socket& m_socket);
    void Start();
private:
    void CheckDeadline();
    void WriteResponse();
    void HandleReq();
    void PreParseGetParam();
private:
    tcp::socket m_socket;

    beast::flat_buffer m_buffer{8192};  //用来接收数据
    http::request<http::dynamic_body> m_request;  //用来解析请求
    http::response<http::dynamic_body> m_response;//用来回应客户端
    //用来做定时器判断请求是否超时
    asio::steady_timer deadline_{
        m_socket.get_executor(), std::chrono::seconds(60) };

    std::string m_get_url;
    std::unordered_map<std::string, std::string> m_get_params;
};



#endif //HTTPCONN_H
