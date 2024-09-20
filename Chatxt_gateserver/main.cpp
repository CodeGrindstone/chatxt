//
// Created by 31435 on 2024/9/4.
//
#include "head.h"
#include "CServer/CServer.h"
#include <iostream>

#include "ConfigMgr/ConfigMgr.h"
int main()
{
    std::string gate_port_str = ConfigMgr::getInstance()["GateServer"]["Port"];
    try
    {
        unsigned short port = atoi(gate_port_str.c_str());
        asio::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
            });
        std::make_shared<CServer>(ioc, port)->Start();
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
