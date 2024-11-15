#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <cstdint>
#include <ctime>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>
#include "LogManager.h"
#include "Socket.h"

// Zues自定义协议包结构
struct TcpPacket{
    uint32_t length;   // 包长度（4 bytes）
    uint16_t func_id;     // 包类型（2 bytes）
    uint8_t version;   // 版本号（1 byte）
    uint8_t reserved;  // 保留字段（1 byte）
    // 以上为包头 总共8字节
    char body[4096];       // 包体，动态长度
    TcpPacket(uint32_t length_value, uint16_t func_id_value, uint8_t version_value, uint8_t reserved_value, const std::string& body_value) :
        length(length_value),
        func_id(func_id_value),
        version(version_value),
        reserved(reserved_value){ 
        memcpy(&body[0], body_value.c_str(), length_value);
    }
    TcpPacket() : length(0), func_id(0), version(0), reserved(0) {}
    char* serializeconstPacket() const {
        // 包头 8 字节 + 包体长度
        uint32_t totalLength = 8 + length;
        char* buffer = new char[totalLength];  // 动态分配内存用于存储序列化后的字节流
        // 初始化位置指针
        char* ptr = buffer;
        // 复制 length（4 字节）
        memcpy(ptr, &length, sizeof(length));
        ptr += sizeof(length);
        // 复制 func_id（2 字节）
        memcpy(ptr, &func_id, sizeof(func_id));
        ptr += sizeof(func_id);
        // 复制 version（1 字节）
        memcpy(ptr, &version, sizeof(version));
        ptr += sizeof(version);
        // 复制 reserved（1 字节）
        memcpy(ptr, &reserved, sizeof(reserved));
        ptr += sizeof(reserved);
        memcpy(ptr, body, length);
        return buffer;  // 返回字节流
    }
    std::string serializeconstPacketToString() const {
        // 包头 8 字节 + 包体长度
        uint32_t totalLength = 8 + length;
        char* buffer = new char[totalLength];  // 动态分配内存用于存储序列化后的字节流
        // 初始化位置指针
        char* ptr = buffer;
        // 复制 length（4 字节）
        memcpy(ptr, &length, sizeof(length));
        ptr += sizeof(length);
        // 复制 func_id（2 字节）
        memcpy(ptr, &func_id, sizeof(func_id));
        ptr += sizeof(func_id);
        // 复制 version（1 字节）
        memcpy(ptr, &version, sizeof(version));
        ptr += sizeof(version);
        // 复制 reserved（1 字节）
        memcpy(ptr, &reserved, sizeof(reserved));
        ptr += sizeof(reserved);
        memcpy(ptr, body, length);
        return std::string(buffer, totalLength);;  // 返回字节流
}

};
enum class ContentType { Text};

enum class MessageType {
    Chat = 0x01,
    LogRequest = 0x02,
    LogResponse = 0x03,
    RegRequest = 0x04,
    RegResponse = 0x05,
    SendVerifyCode = 0x06,
    RetVerifyCode = 0x07,
    AddFriendRequest = 0x08,
    AddFriendResponeMessage = 0x09,
    DeleteFriend = 0xa,
    FriendConfirm = 0xb, 
    InformFriendApply = 0xc,
    CheckPersonalInfo = 0xd,
    CacheInfo = 0x0e,
};

enum class MessageStatus {
    Created, // 消息已经创建
    PendingSend, // 准备发送
    Sent, // 已经发送
    Received, // 已经收到
    PendingProcess, // 准备处理
    Processing, // 正在处理
    Processed, // 处理完成
    SendFailed, // 发送失败
    ReceiveFailed, // 接收失败
    ProcessingError, // 处理失败
    Acknowledged, // 确认
    AckTimeout // 确认超时
};

struct UserConnectInfo {
    bool is_online;
    Zues::Socket::ptr client;
    UserConnectInfo(Zues::Socket::ptr c, bool online) : client(c), is_online(online) {}
    UserConnectInfo() {
    }
};

struct PersonalPublicInfo {
    std::string username;
    uint64_t id;
    std::string email;
    int age;
    PersonalPublicInfo(const std::string& name, uint64_t id_value , int age_value, const std::string& email_value = " ") : 
        username(name),
        id(id_value),
        age(age_value),
        email(email_value){}
    PersonalPublicInfo() :
        username(" "),
        id(0),
        age(0),
        email(""){
    }
    friend void to_json(nlohmann::json& j, const PersonalPublicInfo& p) {
        j = nlohmann::json{{"username", p.username}, {"age", p.age}, {"email", p.email}, {"id", p.id}};
    }
    friend void from_json(const nlohmann::json& j, PersonalPublicInfo& p) {
        j.at("username").get_to(p.username);
        j.at("age").get_to(p.age);
        j.at("email").get_to(p.email);
        j.at("id").get_to(p.id);
    }
};

struct PersonalPrivateInfo {
    std::string password;
    std::vector<PersonalPublicInfo> friend_list;
    std::vector<PersonalPublicInfo> friend_apply;
    PersonalPrivateInfo(const std::string& password_value, const std::vector<PersonalPublicInfo>& friend_list_value, const std::vector<PersonalPublicInfo>& friend_apply_value) :
        password(password_value),
        friend_list(friend_list_value),
        friend_apply(friend_apply_value) {
    }
    PersonalPrivateInfo() :
        password(" "){
    }

    friend void to_json(nlohmann::json& j, const PersonalPrivateInfo& p) {
        j = nlohmann::json{{"password", p.password},
                            {"friend_list", p.friend_list},
                            {"friend_apply", p.friend_apply},
                            };
    }
    friend void from_json(const nlohmann::json& j, PersonalPrivateInfo& p) {
        j.at("password").get_to(p.password);
        j.at("friend_apply").get_to(p.friend_apply);
        j.at("friend_listg").get_to(p.friend_list);
    }
};

class MessageBase {
public:
    MessageBase(ContentType contenttype, MessageStatus status)
        : messageid_(generateMessageId()), contenttype_(contenttype), status_(status), timestamp_(std::time(nullptr)) {}
    virtual ~MessageBase() = default;
    virtual std::string getType() const = 0;
    virtual nlohmann::json toJson() const = 0;
    virtual void castFromString(const std::string& source) = 0;
    static std::string generateMessageId();
protected:
    std::string messageid_;
    ContentType contenttype_;
    MessageStatus status_;
    std::time_t timestamp_;
};

#endif // MESSAGEBASE_H
