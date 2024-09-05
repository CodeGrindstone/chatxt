//
// Created by 31435 on 2024/9/4.
//

#ifndef CSERVER_H
#define CSERVER_H

#include "../head.h"

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(asio::io_context& ioc, unsigned short port);
    void Start();

private:
    asio::io_context& m_ioc;
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};

#endif //CSERVER_H
