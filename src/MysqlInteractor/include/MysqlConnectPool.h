#ifndef MYSQLCONNECTPOOL_H
#define MYSQLCONNECTPOOL_H

#include <jdbc/mysql_connection.h>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "LogManager.h"

#define GET_MYSQL_CON(time)  MysqlConnectPool::getInstance().getConnection(time)

struct MySqlInfo {
    std::string host;
    std::string user;
    std::string password;
    std::string database;
};
struct UsersTableData {
    UsersTableData() {
        username = "";
        password = "";
        email = "";
    }
    UsersTableData(const std::string& username_value, const std::string& password_value, const std::string& email_value) {
        username  = username_value;
        password = password_value;
        email = email_value;

    }
    std::string username;
    std::string email;
    uint64_t id;
    std::string password;
};

struct FriendShipTableData {
    uint64_t friendship_1;
    uint64_t friendship_2;
    std::string status;
    FriendShipTableData() {
        friendship_1 = 0;
        friendship_2 = 0;
        status = "pending";
    }
    FriendShipTableData(uint64_t id1, uint64_t id2, const std::string& status_value) {
        friendship_1 = id1;
        friendship_2 = id2;
        status = status_value;
    }
};
class MysqlConnectPool { 
public:
    static MysqlConnectPool& getInstance(const MySqlInfo& info = MySqlInfo(), int max_connect = 10);
    std::shared_ptr<sql::Connection> getConnection(const int& time_out);
    void releaseConnection(std::shared_ptr<sql::Connection> connection);
    MysqlConnectPool(const MysqlConnectPool&) = delete;
    MysqlConnectPool operator=(const MysqlConnectPool&) = delete;
private:
    std::queue<std::shared_ptr<sql::Connection>> connections_;
    std::shared_ptr<sql::Connection> createConnection();
    MysqlConnectPool(const MySqlInfo& info, int max_connect);

    MySqlInfo info_;
    int max_connect_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

#endif // MYSQLCONNECTPOOL_H


