#include "MysqlConnectPool.h"
#include "MysqlConfigManager.h"
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>


MysqlConnectPool::MysqlConnectPool(const MySqlInfo& info, int max_connect) : info_(info), max_connect_(max_connect) {
    for (int i = 0 ; i < max_connect_; i++) {
        connections_.push(createConnection());
    }
}

MysqlConnectPool& MysqlConnectPool::getInstance(const MySqlInfo& info, int max_connect) {
    static MysqlConnectPool sql_con_pool = MysqlConnectPool(SQL_GET_SQL_INFO, max_connect);
    return sql_con_pool;
}
 
std::shared_ptr<sql::Connection> MysqlConnectPool::getConnection(const int& time_out) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (cv_.wait_for(lock, std::chrono::milliseconds(time_out), [this] { return!connections_.empty();})) {
        auto connection = connections_.front();
        connections_.pop();
        return connection;
    } else {
        return nullptr;
    }
}

void MysqlConnectPool::releaseConnection(std::shared_ptr<sql::Connection> connection) {
    std::unique_lock<std::mutex> lock(mutex_);
    connections_.push(connection);
    cv_.notify_one();
}

std::shared_ptr<sql::Connection> MysqlConnectPool::createConnection() {
    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::cout << "host :" << info_.host << " usr :" << info_.user << " password :" << info_.password << "database :" << info_.database << std::endl;
        auto connect = std::shared_ptr<sql::Connection>(driver->connect(info_.host, info_.user, info_.password), 
            [](sql::Connection* conn) { delete conn; }); 
        connect->setSchema(info_.database);
        return connect;
    } catch(sql::SQLException e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return nullptr;
    }
}
