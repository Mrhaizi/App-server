#include <string>
#include "MysqlInteractor.h"
#include "Logger.h"
#include "LogManager.h"


UsersDao::UsersDao() : BaseDao("users") {
}

UsersDao::~UsersDao() {
}

std::map<std::string, std::string> UsersDao::mapRecordToColums(const UsersTableData &record) {
    return {
        {"username", record.username},
        /* {"id", std::to_string(record.id)}, */
        {"password", record.password},
        {"email", record.email},
    };
}
UsersTableData UsersDao::selectInfoDueUserName(const std::string& user_name) {
    std::string query = "SELECT * FROM " + table_name_ + " WHERE username = ?";
    auto result = select(query, user_name);
    if (result) {
        UsersTableData result_data;
        while (result->next()) {
            result_data.id  = result->getInt("id");
            result_data.username = result->getString("username");
            result_data.password = result->getString("password");
        }
        return result_data;
    } else {
        std::cout << "查询用户失败！" << std::endl;
        return UsersTableData();
    }
}
UsersTableData UsersDao::selectInfoDueId(uint64_t id) {
    std::string query = "SELECT * FROM " + table_name_ + " WHERE id = ?";
    auto result = select(query, std::to_string(id));
    if (result) {
        UsersTableData result_data;
        while (result->next()) {
            result_data.id  = result->getInt("id");
            result_data.username = result->getString("username");
            result_data.password = result->getString("password");
        }
        return result_data;
    } else {
        std::cout << "查询用户失败！" << std::endl;
        return UsersTableData();
    }
}

std::vector<PersonalPublicInfo> UsersDao::getAllUserPublicInfo() {
    std::vector<PersonalPublicInfo> publicinfo_cache;
    std::string query = "SELECT * FROM " + table_name_;
    auto result = select(query);
    if (result) {
        while (result->next()) {
            publicinfo_cache.push_back(PersonalPublicInfo(result->getString("username"), result->getUInt64("id"), 18, result->getString("email")));
        }
        return publicinfo_cache;
    } else {
        std::cout << "查询用户失败！" << std::endl;
        return publicinfo_cache;
    }
}

FriendShipDao::FriendShipDao() : BaseDao("friendships") {}

std::map<std::string, std::string> FriendShipDao::mapRecordToColums(const FriendShipTableData& record) {
    return {
        {"user_id1", std::to_string(record.friendship_1)},
        {"user_id2", std::to_string(record.friendship_2)},
        {"status", record.status},
    };
}

std::shared_ptr<sql::ResultSet> FriendShipDao::select2(const std::string &query, uint64_t param1, uint64_t param2) {
    try {
        auto pstmt = prepareStatement(query);
        if (pstmt) {
            pstmt->setUInt64(1, param1); // 设置第一个参数
            pstmt->setUInt64(2, param2); // 设置第二个参数
            return std::shared_ptr<sql::ResultSet>(pstmt->executeQuery());
        }
    } catch (sql::SQLException& e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return nullptr;
}
std::vector<uint64_t> FriendShipDao::selectAllFriendShip(uint64_t id) {
    std::string query = "SELECT user_id1, user_id2 FROM " + table_name_ + " WHERE ( user_id1 = ? OR user_id2 = ?) AND status = 'accepted'";
    auto result = select2(query, id, id); // 使用两个 id 参数
    std::vector<uint64_t> friend_ids;
    if (result) {
        while (result->next()) {
            uint64_t friend_id;
            if (result->getUInt64("user_id1") == id) {
                // 如果 user_id1 是当前用户ID，那么好友ID是 user_id2
                friend_id = result->getUInt64("user_id2");
            } else {
                // 否则好友ID是 user_id1
                friend_id = result->getUInt64("user_id1");
            }
            ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "id : " << friend_id;
            friend_ids.push_back(friend_id);
        }
    } else {
        ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "check error";
    }
    return friend_ids;
}

bool FriendShipDao::changeFriendShipStatus(uint64_t friend_id, uint64_t own_id, std::string status) {
    FriendShipTableData temp(friend_id, own_id, status);
    std::string condition = "(user_id1 = " + std::to_string(friend_id) + " AND user_id2 = " + std::to_string(own_id) + ") OR (user_id1 = " + std::to_string(own_id) + " AND user_id2 = " + std::to_string(friend_id) + ")";
    return update(temp, condition);
} 

bool FriendShipDao::removeFriendship(uint64_t friend_id, uint64_t own_id) {
        // 构建 WHERE 条件，确保两个 ID 满足任意一个组合
        std::string condition = "(user_id1 = " + std::to_string(friend_id) + " AND user_id2 = " + std::to_string(own_id) +
                                ") OR (user_id1 = " + std::to_string(own_id) + " AND user_id2 = " + std::to_string(friend_id) + ")";
        return remove(condition);
}

std::vector<uint64_t> FriendShipDao::selectAllFriendApply(uint64_t id) {
    std::string query = "SELECT user_id1, user_id2 FROM " + table_name_ + " WHERE ( user_id1 = ? OR user_id2 = ?) AND status = 'pending'";
    auto result = select2(query, id, id); // 使用两个 id 参数
    std::vector<uint64_t> friend_ids;
    if (result) {
        while (result->next()) {
            uint64_t friend_id;
            if (result->getUInt64("user_id1") == id) {
                continue;
            } else {
                // 否则好友ID是 user_id1
                friend_id = result->getUInt64("user_id1");
            }
            friend_ids.push_back(friend_id);
        }
    } else {
        ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "check error";
    }
    return friend_ids;
}

FriendShipDao::~FriendShipDao(){}

