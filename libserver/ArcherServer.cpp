#include "ArcherServer.h"

using namespace fs::server;

static std::function<void(HttpRequest *req, HttpResponse *res)> fileServerOnRequestCallback;

void file_server_message_handler_cb(HttpRequest *req, HttpResponse *res) {
    fileServerOnRequestCallback(req, res);
}

ArcherServer::ArcherServer() {
    m_baseUriList.push_back("/archer");
    m_baseUriList.push_back("/archer/");
    auto instance = common::GlobalConfig::instance();
    if(instance->httpServerEnabledSsl()) {
        m_ssloption = ssl_option_new(0, 0);
        std::ifstream crt(instance->fetchHTTPServerCrtPath());
        if(!crt.is_open()) {
            console_err("Can not open file %s", instance->fetchHTTPServerCrtPath().c_str());
            exit(0);
        }
        std::ifstream key(instance->fetchHTTPServerKeyPath());
        if(!key.is_open()) {
            console_err("Can not open file %s", instance->fetchHTTPServerKeyPath().c_str());
            exit(0);
        }
        std::stringstream crtBuffer, keyBuffer; 
        crtBuffer << crt.rdbuf();
        keyBuffer << key.rdbuf();
        std::string crtStr =  crtBuffer.str(), keyStr = keyBuffer.str();
        ssl_option_set_certificate_and_key(m_ssloption, crtStr.c_str(), crtStr.length(), keyStr.c_str(), keyStr.length());
        crt.close();
        key.close();
        if(!instance->fetchHTTPServerEnCrtPath().empty() && !instance->fetchHTTPServerEnKeyPath().empty()) {
            bool ok = true;
            std::ifstream enCrt(instance->fetchHTTPServerEnCrtPath()), enKey(instance->fetchHTTPServerEnKeyPath());
            if(!enCrt.is_open()) {
                console_warn("Can not open file %s", instance->fetchHTTPServerEnCrtPath().c_str());
                ok = false;
            }
            if(!enKey.is_open()) {
                console_warn("Can not open file %s", instance->fetchHTTPServerEnKeyPath().c_str());
                ok = false;
            }
            if(ok) {
                std::stringstream enCrtBuffer, enKeyBuffer; 
                enCrtBuffer << enCrt.rdbuf();
                enKeyBuffer << enKey.rdbuf();
                std::string enCrtStr =  enCrtBuffer.str(), enKeyStr = enKeyBuffer.str();
                ssl_option_set_encrypt_certificate_and_key(m_ssloption, enCrtStr.c_str(), enCrtStr.length(), enKeyStr.c_str(), enKeyStr.length());
            }
            enCrt.close();
            enKey.close();
        }
        m_http = http_server_new_with_ssl(m_ssloption);
    } else {
        m_http = http_server_new();
    }
}

ArcherServer::~ArcherServer() {
    http_server_close(m_http);
    http_server_free(m_http);
    if(m_ssloption) {
        ssl_option_free(m_ssloption);
    }
}


void ArcherServer::listen(std::string const& host, std::uint16_t const& port) {

    http_server_set_message_handler(m_http, file_server_message_handler_cb);

    ArcherServer *self = this;
    fileServerOnRequestCallback = [self](HttpRequest *req, HttpResponse *res) {
        ServerRequest request(req, res);
        // if(self->checkIndexUri(request.getUri())) {
        //     self->sendIndexHtml(res);
        //     return ;
        // }
        // if(self->checkIconUri(request)) {
        //     self->sendIcon(res);
        //     return ;
        // }
        std::shared_ptr<HttpHandler> handler = self->findHandler(request);
        if(!handler) {
            LOG_warn("Handler not found -Method=%s, -Uri=%s", request.getMethod().c_str(), request.getUri().c_str());
            request.sendNotFound();
            return ;
        }
        if(!handler->auth(request)) {
            LOG_warn("Authonrize failed -Method=%s, -Uri=%s", request.getMethod().c_str(), request.getUri().c_str());
            request.sendAuthFailed();
            return ;
        }
        handler->handleRequest(request);
    };

    if(m_httpThreadNum > 0) {
        http_server_use_threads_pool(m_http, m_httpThreadNum);
    }
    
    console_out("HTTP Server listenning on %s:%d", host.c_str(), port);
    LOG_info("HTTP Server listenned on %s:%d", host.c_str(), port);
    if(!http_server_listen(m_http, host.c_str(), port)) {
        const char *errstr = http_server_get_errstr(m_http);
        console_err("HTTP Server listen on %s:%d error, %s", host.c_str(), port, errstr);
        LOG_error("HTTP Server listen on %s:%d error, %s", host.c_str(), port, errstr);
    }
}

void ArcherServer::close() {
    http_server_close(m_http);
}

void ArcherServer::useMultiThreads(uint16_t const& threadNum) {
    m_httpThreadNum = threadNum;
}

void ArcherServer::addHandler(std::shared_ptr<HttpHandler> const& handler) {
    for(std::string const& s: handler->allUris()) {
        m_baseUriList.push_back(s);
    }
    m_handlerList.push_back(handler);
}


bool ArcherServer::addProxyHandler(std::shared_ptr<handler::ProxyHandler> const& proxy) {
    std::string path = proxy->getRequestPath();
    for(std::string const& s: m_baseUriList) {
        if(s == path || s == (path + '/')) {
            return false;
        }
    }
    for(std::shared_ptr<handler::ProxyHandler> & h: m_proxyList) {
        if(h->getRequestPath() == path || h->getRequestPath() == (path + '/')) {
            return false;
        }
    }
    m_proxyList.push_back(proxy);
    return true;
}

void ArcherServer::removeProxyHandler(std::string const& id) {
    m_proxyList.erase(std::remove_if(m_proxyList.begin(), m_proxyList.end(), [&id](std::shared_ptr<handler::ProxyHandler> & h) { return h->id() == id; }),
                    m_proxyList.end());
}

std::shared_ptr<HttpHandler> ArcherServer::findHandler(ServerRequest & request) {
    for(const auto& h: m_handlerList) {
        if(h->match(request)) {
            return h;
        }
    }
    LOG_info("Request -Method=%s, -Uri=%s", request.getMethod().c_str(), request.getUri().c_str());    
    for(const auto& h: m_proxyList) {
        if(h->match(request)) {
            return h;
        }
    } 
    return std::shared_ptr<HttpHandler>();
}


// void ArcherServer::sendIcon(HttpResponse *res) {
//     http_response_set_status(res, 200);
//     http_response_set_content_type(res, "image/icon");
//     const char *body = fs::common::ICON_SRC;
//     size_t len = fs::common::ICON_SRC_LEN;
//     http_response_send_response(res, body, len);
// }


// void ArcherServer::sendIndexHtml(HttpResponse *res) {
//     http_response_set_status(res, 200);
//     http_response_set_content_type(res, "text/html");
//     std::vector<uint8_t> content = fs::common::base64Decode(INDEX_HTML);
//     http_response_send_response(res, (const char *)content.data(), content.size()-1);
// }

