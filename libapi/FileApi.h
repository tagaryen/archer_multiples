
#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libserver/ServerRequest.h>
#include <libhandler/HttpHandler.h>
#include <libservice/FileService.h>

#include <chrono>

namespace fs 
{
namespace api 
{
class FileApi : public server::HttpHandler
{
public:
    FileApi(service::FileService & fileService, std::string const& authKey):m_service(fileService) {
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
    ~FileApi() {}

    void handleRequest(server::ServerRequest & request) override;

    
    bool match(server::ServerRequest & request) override {
        std::string method = request.getMethod(), uri = request.getUri();
        if(method == "GET" && (uri == m_listUri || uri == m_downloadUri || uri == m_viewUri)) {
            return true;
        } else if(method == "POST" && uri == m_uploadUri) {
            return true;
        }
        return false;
    };

    std::vector<std::string> allUris() override {
        return std::vector<std::string>({m_listUri, m_uploadUri, m_downloadUri, m_viewUri});
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

    std::string m_listUri = "/archer/file-api/file-list";
    std::string m_uploadUri = "/archer/file-api/file-upload";
    std::string m_downloadUri = "/archer/file-api/file-download";
    std::string m_viewUri = "/archer/file-api/file-view";

    uint8_t m_key[16];
    bool m_needAuth;

    service::FileService & m_service;

    void listFile(server::ServerRequest & request);
    void uploadFile(server::ServerRequest & request);
    void downloadFile(server::ServerRequest & request);
    void viewFile(server::ServerRequest & request);
};
}
}
