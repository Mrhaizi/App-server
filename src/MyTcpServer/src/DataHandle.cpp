#include "DataHandle.h"
#include "ChatMessage.h"
#include "MessageBase.h"
#include "CacheMessage.h"
#include "MysqlInteractor.h"


DataHandleMachine::DataHandleMachine() : m_email_sender(nullptr) {
    m_email_sender = std::make_shared<Zues::EmailSender>("2127800097@qq.com", "yqjmdypcycfkegef", "smtps://smtp.qq.com:465");
}
void DataHandleMachine::handleData(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    if (packet.func_id == 0x01) {
        handleChatMessage(packet, client,server);
    } else if (packet.func_id == 0x02) {
        handleLoginMessage(packet,client, server);
    } else if (packet.func_id == 0x04) {
        handleRegMessage(packet,client, server);
    } else if (packet.func_id == 0x06) {
        handleVerifyCode(packet, client, server);
    } else if (packet.func_id == 0x08) {
        handleAddFriendReq(packet, client, server);
    } else if (packet.func_id == 0xa) {
        handleDeleteFriend(packet, client, server);
    } else if (packet.func_id == 0x0b) {
        handleFriendConfirm(packet, client, server);
    }
}

void DataHandleMachine::handleVerifyCode(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    VerifyCodeMessage v_code_msg = VerifyCodeMessage();
    v_code_msg.castFromString(packet.body);
    ZUES_LOG_STREAM_INFO(ZUES_GET_LOG("normal")) << v_code_msg.getEmail();
    Zues::MutexLock lock(m_mutex);
    lock.lock();
    std::string code = m_email_sender->generateVerificationCode();
    lock.unlock();
    if (m_email_sender->sendEmail(v_code_msg.getEmail(), code)) {
        VerifyCodeResMessage v_code_res_msg = VerifyCodeResMessage(code);
        std::string body = v_code_res_msg.toJson().dump();
        TcpPacket packet_res(body.size(), 0x07, 55, 66, body);
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, packet_res.serializeconstPacketToString()));
    } else {
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, "0"));
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "send error";
    }
}

void DataHandleMachine::handleChatMessage(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    ChatMessage rec_msg = ChatMessage();
    rec_msg.castFromString(packet.body);
    auto sender_client = UserConnectionManager::singleton::getInstance()->selectUser(rec_msg.getUidReceiver());
    if (sender_client) {
        std::string re_body = rec_msg.toJson().dump();
        ZUES_LOG_STREAM_INFO(ZUES_GET_LOG("normal")) << "have online : " << re_body;
        TcpPacket packet_res(re_body.size(), 0x01, 55, 66, re_body);
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, sender_client, packet_res.serializeconstPacketToString()));
    } else {
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, packet.serializeconstPacketToString()));
    }
}

void DataHandleMachine::handleLoginMessage(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    LoginRequestMessage l_req_msg = LoginRequestMessage();
    l_req_msg.castFromString(packet.body);
    auto result = UsersDao::singleton::getInstance()->selectInfoDueUserName(l_req_msg.getUsername());
    if (result.password == l_req_msg.getPassword()) {
        PersonalPublicInfo public_info(result.username, result.id, 18, result.email);
        std::vector<PersonalPublicInfo>  friend_list;
        std::vector<PersonalPublicInfo> friend_apply_list;
        auto friend_list_result = FriendShipDao::singleton::getInstance()->selectAllFriendShip(result.id);
        for (auto it : friend_list_result) {
            auto friend_info = UsersDao::singleton::getInstance()->selectInfoDueId(it);
            PersonalPublicInfo public_info(friend_info.username, friend_info.id, 18, friend_info.email);
            friend_list.push_back(public_info);
        }
        auto friend_apply = FriendShipDao::singleton::getInstance()->selectAllFriendApply(result.id);
        for (auto value : friend_apply) {
            auto friend_info = UsersDao::singleton::getInstance()->selectInfoDueId(value);
            PersonalPublicInfo friend_apply_public_info(friend_info.username, friend_info.id, 18, friend_info.email);
            friend_apply_list.push_back(friend_apply_public_info);
        }
        PersonalPrivateInfo private_info(result.password, friend_list, friend_apply_list);
        std::vector<PersonalPublicInfo> pulbic_info_cache = UsersDao::singleton::getInstance()->getAllUserPublicInfo();
        CacheMessage cache_msg(pulbic_info_cache);
        LoginResponseMessage l_res_msg = LoginResponseMessage(public_info, private_info, false);
        l_res_msg.setSuccessful();        
        std::string body = l_res_msg.toJson().dump();
        std::string body_cache = cache_msg.toJson().dump();
        ZUES_LOG_STREAM_INFO(ZUES_GET_LOG("normal")) << "body_cache: " << body_cache;
        TcpPacket packet_res(body.size(), 0x03, 55, 66, body);
        TcpPacket packet_cache(body_cache.size(), 0x0e, 55, 66, body_cache);
        UserConnectionManager::singleton::getInstance()->addUser(result.id,client);
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, packet_res.serializeconstPacketToString()));
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, packet_cache.serializeconstPacketToString()));
    } else {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "error  password";
    }
}
void DataHandleMachine::handleRegMessage(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    RegisterRequestMessage r_req_msg = RegisterRequestMessage();
    r_req_msg.castFromString(packet.body);
    UsersTableData users_table_data(r_req_msg.getUsername(), r_req_msg.getPassword(), r_req_msg.getEmail());
    if (UsersDao::singleton::getInstance()->insert(users_table_data)) {
        RegisterResponseMessage r_res_msg = RegisterResponseMessage(true);
        std::string body = r_res_msg.toJson().dump();
        TcpPacket packet_res(body.size(), 0x05, 55, 66, body);
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, packet_res.serializeconstPacketToString()));
    } else {
        RegisterResponseMessage r_res_msg = RegisterResponseMessage(false);
        std::string body = r_res_msg.toJson().dump();
        TcpPacket packet_res(body.size(), 0x05, 55, 66, body);
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, client, packet_res.serializeconstPacketToString()));
    }
}
void DataHandleMachine::handleAddFriendReq(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    AddFriendRequestMessage addfriend_req = AddFriendRequestMessage();
    addfriend_req.castFromString(packet.body);
    FriendShipTableData data = FriendShipTableData(addfriend_req.getOwnId(), addfriend_req.getFriendId(), "pending");
    FriendShipDao::singleton::getInstance()->insert(data);
    auto send_client = UserConnectionManager::singleton::getInstance()->selectUser(addfriend_req.getFriendId());
    if (send_client->ifConnected()) {
        UsersTableData  data = UsersDao::singleton::getInstance()->selectInfoDueId(addfriend_req.getOwnId());
        PersonalPublicInfo temp(data.username, data.id, 18, data.email);
        InformFriendApplyMessage msg;
        msg.addFriendApply(temp);
        std::string body = msg.toJson().dump();
        TcpPacket packet_res(body.size(), 0xc, 55, 66, body);
        Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::sendMessageToClient, server, send_client, packet_res.serializeconstPacketToString()));
    }
}
void DataHandleMachine::handleDeleteFriend(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    DeleteFriendMessage dele_friend_msg = DeleteFriendMessage();
    dele_friend_msg.castFromString(packet.body);
    if (!FriendShipDao::singleton::getInstance()->removeFriendship(dele_friend_msg.getFriendId(), dele_friend_msg.getOwnId())) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "remove friendship error";
    } 
}
void DataHandleMachine::handleFriendConfirm(const TcpPacket& packet, Zues::Socket::ptr client, MyTcpServer::ptr server) {
    FriendConfirmMessage confirm_msg = FriendConfirmMessage();
     ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << packet.body;
    confirm_msg.castFromString(packet.body);
    if (confirm_msg.ifaccept()) {
        if (!FriendShipDao::singleton::getInstance()->changeFriendShipStatus(confirm_msg.getFriendId(), confirm_msg.getOwnId(), "accepted")) {
             ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "add friendship error";
        }            
    } else {
        if (!FriendShipDao::singleton::getInstance()->removeFriendship(confirm_msg.getFriendId(), confirm_msg.getOwnId())) {
            ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "remove friendship error";
        } 
    }
}
