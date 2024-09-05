cmake_minimum_required(VERSION 3.15)

project(ChatxtServer)

set(CMAKE_CXX_STANDARD 11)

set(CServer
        CServer/CServer.cpp
        CServer/CServer.h
        
)

set(HttpConn
HttpConn/HttpConn.cpp
HttpConn/HttpConn.h
)

set(LogicSystem
LogicSystem/Singleton.h
LogicSystem/LogicSystem.h
LogicSystem/LogicSystem.cpp
)

add_executable(chatxt
        main.cpp
        head.h
        ${CServer}
        ${HttpConn}
        ${LogicSystem}
)