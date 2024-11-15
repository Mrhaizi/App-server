#ifndef DATATRACKLE_H
#define DATATRACKLE_H

#include "Singleton.hpp"
#include "MessageBase.h"
#include "Socket.h"
#include "MyTcpServer.h"
#include "ChatMessage.h"
#include "LoginMessage.h"
#include "RegisterMessage.h"
#include "UserConnectionManager.h"
#include "MysqlInteractor.h"
#include "SendVerifyCode.h"

class DataHandleMachine {
public:
    typedef std::shared_ptr<DataHandleMachine> ptr;
    typedef Singleton<DataHandleMachine>singleton;
    typedef Zues::MutexLock MutexType;
    DataHandleMachine();
    void handleData(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
private:
    void handleChatMessage(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    void handleLoginMessage(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    void handleRegMessage(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    void handleVerifyCode(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    void handleAddFriendReq(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    void handleDeleteFriend(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    void handleFriendConfirm(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server);
    MutexType m_mutex;
    Zues::EmailSender::ptr m_email_sender = nullptr; 
};

#endif // DATATRACKLE_H

