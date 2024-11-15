#ifndef USERCONNECTIONMANAGER_H
#define USERCONNECTIONMANAGER_H
#include <memory>
#include "Socket.h"
#include <unordered_map>
#include "MessageBase.h"
#include "Singleton.hpp"
#include "Lock.h"


class UserConnectionManager {
public:
    typedef std::shared_ptr<UserConnectionManager> ptr;
    typedef Singleton<UserConnectionManager> singleton;
    typedef Zues::RWMutexLock MutexType;
    UserConnectionManager();
    void init();
    bool addUser(uint64_t uid, Zues::Socket::ptr client);
    bool deleteUser(uint64_t uid);
    void changeOnline(Zues::Socket::ptr client);
    Zues::Socket::ptr selectUser(uint64_t uid);
private:
    MutexType m_mutex;
    std::unordered_map<uint64_t, UserConnectInfo>  m_connect_info;
};
#endif// USERCONNECTIONMANAGER_H
