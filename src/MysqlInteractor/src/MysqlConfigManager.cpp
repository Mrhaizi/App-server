#include "MysqlConfigManager.h"

MysqlConfig::MysqlConfig() {

}

MysqlConfig& MysqlConfig::getInstance() {
    static MysqlConfig mysql_config;
    return mysql_config;
}
