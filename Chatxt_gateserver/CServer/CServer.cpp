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
    auto& io_context = AsioIOServicePool::GetInstance()->getIOService();
    std::shared_ptr<HttpConn> new_con = std::make_shared<HttpConn>(io_context);
    m_acceptor.async_accept(new_con->m_socket, [self, new_con](std::error_code ec)
    {
       try
       {
           //出错放弃连接
           if (ec)
           {
               self->Start();
               return;
           }
           //继续监听
           new_con->Start();

           self->Start();
       }catch (std::exception& e)
       {
           std::cout << e.what() << std::endl;
           return;
       }
    });
}
