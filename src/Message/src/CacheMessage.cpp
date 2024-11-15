#include "CacheMessage.h"

CacheMessage::CacheMessage(const std::vector<PersonalPublicInfo>& public_info):
    MessageBase(ContentType::Text, MessageStatus::Created) {
    info_.public_info_cache = public_info;
}

nlohmann::json CacheMessage::toJson() const {
    return nlohmann::json { 
    {"public_info_cache", info_.public_info_cache},
    {"timestamp", timestamp_},
    {"messageid", messageid_},
    {"messagetype", getType()},
    {"status", static_cast<int>(status_)}
};
}
void  CacheMessage::castFromString(const std::string& source) {
    try {
        nlohmann::json j = nlohmann::json::parse(source);
        info_ = j.get<Info>();
    } catch(std::exception e) {
        // ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("system")) << "AddFriendRequestMessage cast error";
    }
}

std::string CacheMessage::getType() const {
    return "CacheInfo";
}
