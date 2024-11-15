#include "RegisterMessage.h"

RegisterRequestMessage::RegisterRequestMessage(const std::string &username, const std::string &password, const std::string& email_value) 
    : MessageBase(ContentType::Text, MessageStatus::Created) {
    info_.username = username;
    info_.password = password;
    info_.email = email_value;
}

std::string RegisterRequestMessage::getType() const {
    return "RegisterRequestMessage";
}

void RegisterRequestMessage::castFromString(const std::string& source)  {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << "RegisterRequestMessage cast error";
    }
}

nlohmann::json RegisterRequestMessage::toJson() const {
    return nlohmann::json {
    {"messagetype", getType()},
    {"username",  info_.username},
    {"password", info_.password},
    {"email", info_.email},
    {"messageid", messageid_},
    {"timestamp", timestamp_},
    {"conenttype", getType()},
    {"status", static_cast<int>(status_)},
    };
}

RegisterResponseMessage::RegisterResponseMessage(bool success) 
    : MessageBase(ContentType::Text, MessageStatus::Created) {
        info_.success = success;
    }

std::string RegisterResponseMessage::getType() const {
    return "RegisterResponseMessage";
}

nlohmann::json RegisterResponseMessage::toJson() const {
    return nlohmann::json {
    {"messagetype", getType()},
    {"success", info_.success},
    };
}

void RegisterResponseMessage::castFromString(const std::string& source) {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << " void RegisterResponseMessage cast error";
    }
}
VerifyCodeMessage::VerifyCodeMessage(const std::string &email) 
    : MessageBase(ContentType::Text, MessageStatus::Created) {
}

void VerifyCodeMessage::castFromString(const std::string& source) {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << " void RegisterResponseMessage cast error";
    }
}

nlohmann::json VerifyCodeMessage::toJson() const {
    return nlohmann::json {
    {"messagetype", getType()},
    {"email", info_.email},
    };
}

std::string VerifyCodeMessage::getType() const {
    return "VerifyCodeMessage";
}

VerifyCodeResMessage::VerifyCodeResMessage(const std::string &code) 
    : MessageBase(ContentType::Text, MessageStatus::Created) {
    info_.code = code;
}

void VerifyCodeResMessage::castFromString(const std::string& source) {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << " void RegisterResponseMessage cast error";
    }
}

nlohmann::json VerifyCodeResMessage::toJson() const {
    return nlohmann::json {
    {"messagetype", getType()},
    {"code", info_.code},
    };
}

std::string VerifyCodeResMessage::getType() const {
    return "VerifyCodeResMessage";
}
