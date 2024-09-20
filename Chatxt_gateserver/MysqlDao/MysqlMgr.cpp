//
// Created by 31435 on 2024/9/16.
//

#include "MysqlMgr.h"

#include "MysqlMgr.h"
MysqlMgr::~MysqlMgr() {
}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return _dao.RegUser(name, email, pwd);
}

MysqlMgr::MysqlMgr() {
}
