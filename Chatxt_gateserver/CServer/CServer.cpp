//
// Created by 31435 on 2024/9/4.
//

#include "CServer.h"
#include <iostream>

#include "../HttpConn/HttpConn.h"

CServer::CServer(asio::io_context& ioc, unsigned short port):m_ioc(ioc),
                                                             m_acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
                                                             m_socket(ioc)
{

}

void CServer::Start()
{
    auto self = shared_from_this();
    m_acceptor.async_accept(m_socket, [self](std::error_code ec)
    {
       try
       {
           //出错放弃连接
           if (ec)
           {
               self->Start();
               return;
           }
           std::make_shared<HttpConn>(self->m_socket)->Start();
           //继续监听
           self->Start();
       }catch (std::exception& e)
       {
           std::cout << e.what() << std::endl;
           return;
       }
    });
}
