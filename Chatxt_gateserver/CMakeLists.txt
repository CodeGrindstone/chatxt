cmake_minimum_required(VERSION 3.15)

project(ChatxtServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)


# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)

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

set(ConfigMgr
ConfigMgr/ConfigMgr.h
ConfigMgr/ConfigMgr.cpp
)

set(Vefiry
        VerifyGrpcClient/VerifyGrpcClient.h
        VerifyGrpcClient/message.pb.h
        VerifyGrpcClient/message.pb.cc
        VerifyGrpcClient/message.grpc.pb.h
        VerifyGrpcClient/message.grpc.pb.cc
)

set(AsioIOServicePool
AsioIOServicePool/AsioIOServicePool.h
AsioIOServicePool/AsioIOServicePool.cpp
)

set(RedisMgr
RedisMgr/RedisMgr.h
RedisMgr/RedisMgr.cpp
)

set(MysqlDao
MysqlDao/MysqlDao.h
MysqlDao/MysqlDao.cpp
        MysqlDao/MysqlMgr.h
        MysqlDao/MysqlMgr.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_executable(chatxt
        main.cpp
        head.h
        ${CServer}
        ${HttpConn}
        ${LogicSystem}
        ${ConfigMgr}
        ${Vefiry}
        ${AsioIOServicePool}
        ${RedisMgr}
        ${MysqlDao}
)

target_link_libraries(chatxt
        jsoncpp
        boost_filesystem
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        hiredis
        mysqlcppconn
)