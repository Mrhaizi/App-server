#ifndef REGISTERMESSAGE_H
#define REGISTERMESSAGE_H

#include "MessageBase.h"

class VerifyCodeMessage : public MessageBase {
public:
    struct Info {
        std::string email;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{{"email", info.email}};

        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("email").get_to(info.email);

        }
    };
    VerifyCodeMessage(const std::string &email = "");
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    std::string getEmail() {return info_.email;}
private:
    Info info_;
};

class VerifyCodeResMessage : public MessageBase {
public:
    struct Info {
        std::string code;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{{"code", info.code}};

        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("code").get_to(info.code);

        }
    };
    VerifyCodeResMessage(const std::string &code = "");
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    std::string getEmail() {return info_.code;}
private:
    Info info_;
};

class RegisterRequestMessage : public MessageBase  {
public:
    struct Info { 
        std::string username;
        std::string password;
        std::string email;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"password", info.password},
                                {"username", info.username},
                                {"email", info.email}};

        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("username").get_to(info.username),
            j.at("password").get_to(info.password),
            j.at("email").get_to(info.email);

        }
    };
    RegisterRequestMessage(const std::string &username = "", const std::string &password = "", const std::string& email_value = "");
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    std::string getUsername() {return info_.username;}
    std::string getPassword() {return info_.password;}
    std::string getEmail() {return info_.email;}
private:
    Info info_;
};

class RegisterResponseMessage : public MessageBase {
public:
    struct Info {
        bool success;
        /* PersonalPublicInfo public_info; */
        /* PersonalPrivateInfo private_info; */ 
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"success"}, info.success};
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("success").get_to(info.success);
        }
    };
    RegisterResponseMessage(bool success);
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    PersonalPublicInfo getPublicInfo();
    PersonalPrivateInfo getPrivateInfo();
    void setSuccessful() {info_.success = true;} 
private:
    Info info_;
};


#endif // REGISTERMESSAGE_H
