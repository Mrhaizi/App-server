#ifndef CACHEMESSAGE_H
#define CACHEMESSAGE_H

#include "MessageBase.h"
using nlohmann::json;
class CacheMessage : public MessageBase {
public:
    struct Info {
        std::vector<PersonalPublicInfo> public_info_cache;
        friend void to_json(nlohmann::json& j, const Info& info) {
            j = nlohmann::json{ {"public_info_cache", info.public_info_cache},
                                };
        }
        // 从 JSON 解析到 Info
        friend void from_json(const nlohmann::json& j, Info& info) {
            j.at("public_info_cache").get_to(info.public_info_cache);
        }    
    };
    CacheMessage(const std::vector<PersonalPublicInfo>& public_info);
    void printMessage();
    std::string getType() const override;
    nlohmann::json toJson() const override;
    void castFromString(const std::string& source) override;
    std::vector<PersonalPublicInfo> getPersonalPublicInfoCache();
private:
    Info info_;
};

#endif // CACHEMESSAGE_H
