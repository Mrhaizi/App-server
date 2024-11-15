#ifndef MESSAGE_H
#define MESSAGE_H

#include "MessageBase.h"
#include "glob.h"
#include "nlohmann/json.hpp"
#include <cstdint>
using nlohmann::json;
class ChatMessage : public MessageBase {
public:
    struct Info {
        std::string content;
        std::string sender;
        std::string receiver;
        uint64_t uid_receiver;
        uint64_t uid_sender;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"content", info.content},
                                {"sender", info.sender},
                                {"receiver", info.receiver},
                                {"uid_receiver", info.uid_receiver},
                                {"uid_sender", info.uid_sender}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("content").get_to(info.content);
            j.at("sender").get_to(info.sender);
            j.at("receiver").get_to(info.receiver);
            j.at("uid_receiver").get_to(info.uid_receiver);
            j.at("uid_sender").get_to(info.uid_sender);
        }    
    };
    ChatMessage(const std::string &content = "", const std::string &sender = "", const std::string &receiver= "");
    void printMessage();
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    uint64_t getUidSender() {return info_.uid_sender;}
    uint64_t getUidReceiver() {return info_.uid_receiver;}
    std::string getConent() {return info_.content;}
private:
    Info info_;
};

class AddFriendRequestMessage : public MessageBase {
public:
    struct Info {
        std::string own_username;
        std::string friend_username;
        uint64_t friend_id;
        uint64_t own_id;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"friend_username", info.friend_username},
                                {"own_username", info.own_username},
                                {"friend_id", info.friend_id},
                                {"own_id", info.own_id}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("friend_username").get_to(info.friend_username);
            j.at("own_username").get_to(info.own_username);
            j.at("friend_id").get_to(info.friend_id),
            j.at("own_id").get_to(info.own_id);
        }    
    };
    AddFriendRequestMessage(const std::string& friend_username = "", uint64_t friend_id = 0, const std::string& own_username = "", uint64_t own_id = 0);
    void printMessage();
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    uint64_t getFriendId() {return info_.friend_id;}
    uint64_t getOwnId() {return info_.own_id;}
    std::string getFriendUsername() {return info_.friend_username;}
    std::string getOwnUsername() {return info_.own_username;}
private:
    Info info_;
};
class AddFriendResponeMessage: public MessageBase {
public:
    struct Info {
        PersonalPublicInfo public_info;
        bool success;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"public_info", info.public_info},
                                {"success", info.success}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("public_info").get_to(info.public_info);
        }    
    };
    AddFriendResponeMessage(bool success, PersonalPublicInfo public_info);
    void printMessage();
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    PersonalPublicInfo getUidSender() {return info_.public_info;}
    bool getSuccess() {return info_.success;}
private:
    Info info_;
};

class DeleteFriendMessage: public MessageBase {
public:
    struct Info {
        std::string username;
        uint64_t friend_id;
        uint64_t own_id;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"username", info.username},
                                {"friend_id", info.friend_id},
                                {"own_id", info.own_id}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("username").get_to(info.username);
            j.at("friend_id").get_to(info.friend_id),
            j.at("own_id").get_to(info.own_id);
        }    
    };
    DeleteFriendMessage(const std::string& username = "", uint64_t friend_id = 0, uint64_t own_id = 0);
    void printMessage();
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    std::string getUsername() {return info_.username;}
    uint64_t getFriendId() {return info_.friend_id;}
    uint64_t getOwnId() {return info_.own_id;}
private:
    Info info_;
};

class FriendConfirmMessage : public MessageBase {
public:
    struct Info {
        uint64_t friend_id;
        uint64_t own_id;
        bool success;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"success", info.success},
                                {"friend_id", info.friend_id},
                                {"own_id", info.own_id}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("success").get_to(info.success);
            j.at("friend_id").get_to(info.friend_id);
            j.at("own_id").get_to(info.own_id);
        }    
    };
    FriendConfirmMessage(bool success = false, uint64_t friend_id = 0, uint64_t own_id = 0);
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    uint64_t getFriendId() {return info_.friend_id;}
    uint64_t getOwnId() {return info_.own_id;}
    bool ifaccept() {return info_.success;}
private:
    Info info_;
};

class InformFriendApplyMessage : public MessageBase {
public:
    struct Info {
        std::vector<PersonalPublicInfo> apply_public_info;
        uint64_t friend_id;
        std::string username;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"public_info", info.apply_public_info},};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("public_info").get_to(info.apply_public_info);
        }    
    };
    InformFriendApplyMessage(uint64_t friend_id = 0, const std::string& username = "");
    void addFriendApply(PersonalPublicInfo public_info);
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    uint64_t getFriendId() {return info_.friend_id;}
    std::string getUsername() {return info_.username;}
private:
    Info info_;
};

#endif // MESSAGE_H
