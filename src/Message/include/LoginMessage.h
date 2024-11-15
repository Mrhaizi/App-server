#ifndef LOGINMESSAGE_H
#define LOGINMESSAGE_H

#include <MessageBase.h>


class LoginRequestMessage : public MessageBase  {
public:
    struct Info { 
        std::string username;
        std::string password;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"password", info.password},
                                {"username", info.username}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("username").get_to(info.username),
            j.at("password").get_to(info.password);
        }
    };
    LoginRequestMessage(const std::string &username = "", const std::string &password = "");
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    std::string getUsername() {return info_.username;}
    std::string getPassword() {return info_.password;}
private:
    Info info_;
};

class LoginResponseMessage : public MessageBase {
public:
    struct Info {
        bool success;
        PersonalPublicInfo public_info;
        PersonalPrivateInfo private_info; 
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"public_info", info.public_info},
                                {"private_info", info.private_info},
                                {"success", info.success}};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("public_info").get_to(info.public_info),
            j.at("private_info").get_to(info.private_info),
            j.at("success").get_to(info.success);
        }
    };
    LoginResponseMessage(PersonalPublicInfo public_info, PersonalPrivateInfo private_info, bool success);
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    PersonalPublicInfo getPublicInfo();
    PersonalPrivateInfo getPrivateInfo();
    void setSuccessful() {info_.success = true;} 
private:
    Info info_;
};
#endif // LOGINMESSAGE_H
