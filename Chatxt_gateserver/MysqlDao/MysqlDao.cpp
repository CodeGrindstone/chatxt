//
// Created by 31435 on 2024/9/16.
//

#include "MysqlDao.h"

MysqlDao::MysqlDao()
{
    auto & cfg = ConfigMgr::getInstance();
    const auto& host = cfg["Mysql"]["Host"];
    const auto& port = cfg["Mysql"]["Port"];
    const auto& pwd = cfg["Mysql"]["Passwd"];
    const auto& schema = cfg["Mysql"]["Schema"];
    const auto& user = cfg["Mysql"]["User"];
    pool_.reset(new MySqlPool(host+":"+port, user, pwd,schema, 5));
}


MysqlDao::~MysqlDao(){
    pool_->Close();
}


int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    // 从连接池获取一个数据库连接
    auto con = pool_->getConnection();

    try {
        // 检查连接是否成功获取
        if (con == nullptr) {
            // 如果连接获取失败，将空连接归还到池中并返回错误
            pool_->returnConnection(std::move(con));
            return false;
        }

        // 准备调用存储过程的 SQL 语句
        // 这里假设存储过程名为 reg_user，它有 3 个输入参数和 1 个输出参数
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("CALL reg_user(?,?,?,@result)"));

        // 设置存储过程的输入参数
        stmt->setString(1, name);   // 设置第一个参数：用户名
        stmt->setString(2, email);  // 设置第二个参数：电子邮件
        stmt->setString(3, pwd);    // 设置第三个参数：密码

        // 执行存储过程
        stmt->execute();

        // 创建 Statement 对象来执行查询
        std::unique_ptr<sql::Statement> stmtResult(con->createStatement());

        // 查询存储过程执行后的结果
        // 假设存储过程通过会话变量 @result 返回结果
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));

        // 处理查询结果
        if (res->next()) {
            int result = res->getInt("result"); // 获取查询结果中的整数值
            std::cout << "Result: " << result << std::endl; // 输出结果到控制台
            // 将连接归还到连接池中
            pool_->returnConnection(std::move(con));
            return result; // 返回存储过程的结果
        }

        // 如果没有获取到结果，将连接归还到连接池中，并返回错误代码
        pool_->returnConnection(std::move(con));
        return -1;
    }
    catch (sql::SQLException& e) {
        // 捕获并处理 SQL 异常
        pool_->returnConnection(std::move(con)); // 将连接归还到连接池中
        // 输出异常信息，包括错误代码和 SQL 状态
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1; // 返回错误代码
    }
}

