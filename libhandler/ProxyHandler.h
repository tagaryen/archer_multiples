
#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libserver/ServerRequest.h>
#include <libhandler/HttpHandler.h>
#include <libservice/FileService.h>

#include <map>

namespace fs 
{
namespace handler 
{
class ProxyHandler : public server::HttpHandler
{
public:
    ProxyHandler(std::string const & body);
    ~ProxyHandler() {
        if(m_ssloption) {
            ssl_option_free(m_ssloption);
        }
    };

    void handleRequest(server::ServerRequest & request) override;

    bool match(server::ServerRequest & request) override {
        std::string uri = request.getReferer() + request.getUri();
        size_t len = m_requestPath.length();
        if(uri.length() >= len && strncmp(uri.c_str(), m_requestPath.c_str(), len) == 0) {
            return true;
        }
        return false;
    };
    
    std::vector<std::string> allUris() override {
        return std::vector<std::string>();
    }

    bool auth(server::ServerRequest & request) override {return true;};

    bool isValid() {return m_valid;};

    std::string const& getRequestPath() {return m_requestPath;};
    std::string const& getRaw() {return m_raw;};

private:

    bool parseUrl();
    void handleLocation(server::ServerRequest & request);
    void handleProxy(server::ServerRequest & request);

    bool m_valid;

    bool m_ssl = false;

    std::string      m_raw;

    std::string      m_name;
    std::string      m_requestPath;
    std::string      m_location;
    std::string      m_proxyUrl;
    std::string      m_host;
    std::vector<std::pair<std::string, std::string>>    m_requestHeaders;
    std::vector<std::pair<std::string, std::string>>    m_responseHeaders;
    
    SSLOption       *m_ssloption = NULL;
};
}
}
