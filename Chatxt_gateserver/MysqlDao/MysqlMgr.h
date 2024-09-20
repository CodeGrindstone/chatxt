//
// Created by 31435 on 2024/9/16.
//

#ifndef MYSQLMGR_H
#define MYSQLMGR_H

#include "../head.h"
#include "MysqlDao.h"
#include "../LogicSystem/Singleton.h"

class MysqlMgr: public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email,  const std::string& pwd);
private:
    MysqlMgr();
    MysqlDao  _dao;
};



#endif //MYSQLMGR_H
