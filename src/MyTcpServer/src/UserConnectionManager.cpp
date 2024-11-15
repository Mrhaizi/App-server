#include "UserConnectionManager.h"

UserConnectionManager::UserConnectionManager() {
}

void UserConnectionManager::init() {
    m_connect_info.clear(); // 清除已有的连接信息
}

// 添加用户及其对应的套接字
bool UserConnectionManager::addUser(uint64_t uid, Zues::Socket::ptr client) {
    MutexType::WriteLock lock(m_mutex);
    if (m_connect_info.find(uid) != m_connect_info.end()) {
        return false; // 如果用户已存在，返回false
    }
    m_connect_info[uid] = {client, true}; // 将用户标记为在线并添加
    return true;
}
// 删除指定uid的用户
bool UserConnectionManager::deleteUser(uint64_t uid) {
    MutexType::WriteLock lock(m_mutex);
    return m_connect_info.erase(uid) > 0; // erase返回删除的元素数量
}

// 通过uid查找用户的套接字
Zues::Socket::ptr UserConnectionManager::selectUser(uint64_t uid) {
    MutexType::ReadLock lock(m_mutex);
    auto it = m_connect_info.find(uid);
    if (it != m_connect_info.end()) {
        return it->second.client;
    }
    return nullptr;
}
// 将用户的在线状态更改为true
void UserConnectionManager::changeOnline(Zues::Socket::ptr client) {
    MutexType::WriteLock lock(m_mutex);
    for (auto& pair : m_connect_info) {
        if (pair.second.client == client) {
            pair.second.is_online = true;
            break;
        }
    }
}
