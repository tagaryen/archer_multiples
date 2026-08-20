#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libhandler/HttpHandler.h>
#include <libhandler/ProxyHandler.h>

#include "archer_net.h"

namespace fs 
{
namespace server 
{
class ArcherServer
{
public:

    ArcherServer();
    ~ArcherServer();

    void listen(std::string const& host, std::uint16_t const& port);
    void close();
    void useMultiThreads(uint16_t const& threadNums);
    void addHandler(std::shared_ptr<HttpHandler> const& handler);
    bool addProxyHandler(std::shared_ptr<handler::ProxyHandler> const& proxy);
    void removeProxyHandler(std::string const& id);
    std::shared_ptr<HttpHandler> findHandler(ServerRequest & request);
    bool checkIndexUri(std::string const& uri) {return uri == m_indexUri || uri == m_indexUri2;};
    bool checkIconUri(ServerRequest & request) {
        return request.hasReferer() && checkIndexUri(request.getReferer()) && request.getUri() == m_iconUri;
    };

private:
    // void sendIcon(HttpResponse *res);
    // void sendIndexHtml(HttpResponse *res);

    std::string                                   m_indexUri = "/archer";
    std::string                                   m_indexUri2 = "/archer/";
    std::string                                   m_iconUri = "/favicon.ico";
    HttpServer                                   *m_http;
    SSLOption                                    *m_ssloption = NULL;
    std::vector<std::shared_ptr<HttpHandler>>     m_handlerList;
    uint16_t                                      m_httpThreadNum;
    std::vector<std::string>                      m_baseUriList = {m_indexUri, m_indexUri2};
    
    std::vector<std::shared_ptr<handler::ProxyHandler>> m_proxyList;
};
}
}