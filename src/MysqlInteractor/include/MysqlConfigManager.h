#ifndef MYSQLCONFIGMANAGER_H
#define MYSQLCONFIGMANAGER_H
#include <memory>
#include <string>

#include "MysqlConnectPool.h"

#define SQL_SET_IP(ip) MysqlConfig::getInstance().setIp(ip)
#define SQL_SET_NAME(name) MysqlConfig::getInstance().setUserName(name)
#define SQL_SET_PASSWORD(password) MysqlConfig::getInstance().setPassword(password)
#define SQL_SET_DATABASE(database) MysqlConfig::getInstance().setDatabase(database)
#define SQL_GET_IP MysqlConfig::getInstance().getIp()
#define SQL_GET_NAME MysqlConfig::getInstance().getUserName()
#define SQL_GET_PASSWORD  MysqlConfig::getInstance().getPassword()
#define SQL_GET_DATABASE  MysqlConfig::getInstance().getDatabase()
#define SQL_GET_SQL_INFO  MysqlConfig::getInstance().getSqlInfo()

class MysqlConfig {
public:
    static MysqlConfig& getInstance();
    void setUserName(const std::string& name) { info_.user = name;}
    void setIp(const std::string& ip) {info_.host  = ip;}
    void setPassword(const std::string& password) {info_.password  = password;}
    void setDatabase(const std::string& database) {info_.database  = database;}
    std::string& getUserName() {return info_.user;}
    std::string& getIp() {return info_.host;}
    std::string& getPassword(){return info_.password;}
    std::string& getDatabase() {return info_.database;}
    MySqlInfo& getSqlInfo() {return info_;}
    MysqlConfig(const MysqlConfig&) = delete;
    MysqlConfig operator=(const MysqlConfig&) = delete;
private:

    MysqlConfig();
    MySqlInfo info_;
};
#endif // MYSQLCONFIGMANAGER_H
