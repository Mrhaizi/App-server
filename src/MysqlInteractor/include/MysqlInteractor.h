#ifndef MYSQLINTERACTOR_H
#define MYSQLINTERACTOR_H

#include <cstdint>
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>
#include "MysqlConnectPool.h"
#include "Singleton.hpp"
#include "MessageBase.h"

template<typename T>
class BaseDao {
public:
    BaseDao(const std::string &table_name) : table_name_(table_name) {
        connection_ = GET_MYSQL_CON(5000);
    }
    virtual ~BaseDao() {
        MysqlConnectPool::getInstance(mysql_info_, 10).releaseConnection(connection_);
    }
    bool insert(const T &record) {
        try {
            std::map<std::string, std::string> column_data = mapRecordToColums(record);
            std::string columns, placeholders;
            for (const auto& [key, value] : column_data) {
                columns += key + ", ";
                placeholders += "?, ";
            }
            columns.pop_back(); columns.pop_back(); // 去掉最后的逗号和空格
            placeholders.pop_back(); placeholders.pop_back();
            std::string query = "INSERT INTO " + table_name_ + " (" + columns + ") VALUES (" + placeholders + ")";
            ZUES_LOG_STREAM_INFO(ZUES_GET_LOG("normal")) << query;
            auto pstmt = prepareStatement(query);
            if (pstmt) {
                int index = 1;
                for (const auto& [key, value] : column_data) {
                    if (key == "user_id1" || key == "user_id2") {
                        pstmt->setUInt64(index++, std::stoull(value));
                    } else if (key == "status") {
                        ZUES_LOG_STREAM_INFO(ZUES_GET_LOG("normal")) << "status : " << value;

                        pstmt->setString(index++, value);  // 确保设置了 status 字段
                    }
                }
                pstmt->executeUpdate();
                return true;
            }
        } catch (sql::SQLException& e) {
            std::cerr << "MySQL error: " << e.what() << std::endl;
        }
        return false;
    }
    bool update(const T &record, const std::string &condition) {
        try {
            std::map<std::string, std::string> column_data = mapRecordToColums(record);
            std::string set_clause;
            for (const auto &pair : column_data) {
                set_clause += pair.first + " = ?, ";
            }
            set_clause.pop_back(); set_clause.pop_back(); // 去掉最后的逗号和空格
            std::string query = "UPDATE " + table_name_ + " SET " + set_clause + " WHERE " + condition;
            auto pstmt = prepareStatement(query);
            if (pstmt) {
                int index = 1;
                for (const auto &pair : column_data) {
                    pstmt->setString(index++, pair.second);
                }
                pstmt->executeUpdate();
                return true;
            }
        } catch (sql::SQLException &e) {
            std::cerr << "MySQL error in update: " << e.what() << std::endl;
        }
        return false;
    }
    bool remove(const std::string &condition) {
    try {
        std::string query = "DELETE FROM " + table_name_ + " WHERE " + condition;
        auto pstmt = prepareStatement(query);
        if (pstmt) {
            pstmt->executeUpdate();
            return true;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL error in remove: " << e.what() << std::endl;
    }
    return false;
    }
    std::shared_ptr<sql::ResultSet> select(const std::string &query, const std::string &param) {
        try {
            auto pstmt = prepareStatement(query);
            if (pstmt) {
                pstmt->setString(1, param); // 使用绑定参数，防止 SQL 注入
                return std::shared_ptr<sql::ResultSet>(pstmt->executeQuery());
            }
        } catch (sql::SQLException& e) {
            std::cerr << "MySQL error: " << e.what() << std::endl;
        }
        return nullptr;
    }
    std::shared_ptr<sql::ResultSet> select(const std::string &query) {
        try {
            auto pstmt = prepareStatement(query);
            if (pstmt) {
                return std::shared_ptr<sql::ResultSet>(pstmt->executeQuery());
            }
        } catch (sql::SQLException& e) {
            std::cerr << "MySQL error: " << e.what() << std::endl;
        }
        return nullptr;
    }
protected:
    std::string table_name_;
    virtual std::map<std::string, std::string> mapRecordToColums(const T &record) = 0;

    std::shared_ptr<sql::PreparedStatement> prepareStatement(const std::string& query) {
        try {
            return std::shared_ptr<sql::PreparedStatement>(connection_->prepareStatement(query));
        } catch (sql::SQLException& e) {
            std::cerr << "MySQL prepare statement error: " << e.what() << std::endl;
            return nullptr;
        }
    }
private:
    void closeConnection() {
        try {
            if (connection_) {
                connection_->close();
            }
        } catch (sql::SQLException& e) {
            std::cerr << "MySQL close connection error: " << e.what() << std::endl;
        }

    }
    MySqlInfo mysql_info_;
    std::shared_ptr<sql::mysql::MySQL_Driver> driver_;
    std::shared_ptr<sql::Connection> connection_;
};

class UsersDao : public BaseDao<UsersTableData> {
public:
    typedef Singleton<UsersDao> singleton;
    UsersDao();
    ~UsersDao() override;
    std::map<std::string, std::string> mapRecordToColums(const UsersTableData &record) override;
    UsersTableData selectInfoDueUserName(const std::string& user_name);
    UsersTableData selectInfoDueId(uint64_t id);
    std::vector<PersonalPublicInfo> getAllUserPublicInfo();
};

class FriendShipDao : public BaseDao<FriendShipTableData> {
public:
    typedef Singleton<FriendShipDao> singleton;
    FriendShipDao();
    ~FriendShipDao() override;
    std::shared_ptr<sql::ResultSet> select2(const std::string &query, uint64_t param1, uint64_t param2);
    std::vector<uint64_t> selectAllFriendShip(uint64_t id);
    std::vector<uint64_t> selectAllFriendApply(uint64_t id);
    std::map<std::string, std::string> mapRecordToColums(const FriendShipTableData &record) override;
    bool changeFriendShipStatus(uint64_t friend_id, uint64_t own_id, std::string status);
    bool removeFriendship(uint64_t friend_id, uint64_t own_id);
private:
};
#endif // MYSQLINTERACTOR_H
