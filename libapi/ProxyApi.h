
#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libserver/ServerRequest.h>
#include <libhandler/HttpHandler.h>
#include <libservice/ProxyService.h>

namespace fs 
{
namespace api 
{
class ProxyApi : public server::HttpHandler
{
public:
    ProxyApi(service::ProxyService & service, std::string const& authKey):m_service(service) {
        if(authKey.length() > 16) {
            m_needAuth = true;
            memcpy(m_key, authKey.c_str(), 16);
        } else if(authKey.empty()) {
            m_needAuth = false;
        } else {
            m_needAuth = true;
            memcpy(m_key, authKey.c_str(), authKey.length());
            memset(m_key + authKey.length(), '#', 16  - authKey.length());
        }
    }
    ~ProxyApi() {}

    void handleRequest(server::ServerRequest & request) override;

    bool match(server::ServerRequest & request) override {
        std::string method = request.getMethod(), uri = request.getUri();
        if(method == "GET" && uri == m_listUri) {
            return true;
        } else if(method == "POST" && (uri == m_addUri || uri == m_delUri)) {
            return true;
        }
        return false;
    };
    
    std::vector<std::string> allUris() override {
        return std::vector<std::string>({m_listUri, m_addUri, m_delUri});
    }

    bool auth(server::ServerRequest & request) override {
        if(!m_needAuth) {
            return true;
        }
        std::string signature = request.getQuery("signature");
        if(signature.empty()) {
            return false;
        }
        std::string t = request.getQuery("t");
        long long millis = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
        long long reqTime = std::atoll(t.c_str());
        if(millis - reqTime >= 3 * 60 * 1000) {
            return false;
        }
        std::string rawText = request.getUri() +  t;
        std::vector<uint8_t> sigBytes = common::base64Decode(signature);
        uint8_t *out = NULL;
        size_t len = 0;
        bool ok = false;
        if(sm4_decrypt(m_key, sigBytes.data(), sigBytes.size(), &out, &len)) {
            ok = (len == rawText.length()) && (strncmp((char *) out, rawText.c_str(), len) == 0 );
        }
        if(out) {
            free(out);
        }
        return ok;
    };
private:

    std::string m_listUri = "/archer/proxy-api/proxy-list";
    std::string m_addUri = "/archer/proxy-api/proxy-add";
    std::string m_delUri = "/archer/proxy-api/proxy-del";

    uint8_t m_key[16];
    bool m_needAuth;

    service::ProxyService & m_service;

    void listProxy(server::ServerRequest & request);
    void addProxy(server::ServerRequest & request);
    void delProxy(server::ServerRequest & request);
};
}
}
