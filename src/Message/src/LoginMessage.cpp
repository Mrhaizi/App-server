#include "LoginMessage.h"

LoginRequestMessage::LoginRequestMessage(const std::string &username, const std::string &password) 
    : MessageBase(ContentType::Text, MessageStatus::Created) {
}

std::string LoginRequestMessage::getType() const {
    return "LoginRequestMessage";
}

void LoginRequestMessage::castFromString(const std::string& source)  {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << "LoginRequestMessage cast error";
    }
}

nlohmann::json LoginRequestMessage::toJson() const {
    return nlohmann::json {
    {"messagetype", getType()},
    {"username",  info_.username},
    {"password", info_.password},
    {"messageid", messageid_},
    {"timestamp", timestamp_},
    {"status", static_cast<int>(status_)},
    };
}

LoginResponseMessage::LoginResponseMessage(PersonalPublicInfo public_info, PersonalPrivateInfo private_info, bool success) 
    : MessageBase(ContentType::Text, MessageStatus::Created) {
        info_.public_info = public_info;
        info_.private_info = private_info;
        info_.success = success;
    }

std::string LoginResponseMessage::getType() const {
    return "LoginResponseMessage";
}

nlohmann::json LoginResponseMessage::toJson() const {
    return nlohmann::json {
    {"messagetype", getType()},
    {"public_info", info_.public_info},
    {"private_info", info_.private_info},
    {"success", info_.success},
    };
}

void LoginResponseMessage::castFromString(const std::string& source) {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << "chatMEssage cast error";
    }

}


