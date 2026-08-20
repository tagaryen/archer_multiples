#include "ProxyApi.h"
#include <libservice/FileService.h>

using namespace fs::api;


void ProxyApi::handleRequest(server::ServerRequest & request) {
    std::string uri = request.getUri();
    std::string method = request.getMethod();

    if(uri == m_listUri) {
        listProxy(request);
    } else if(uri == m_addUri) {
        addProxy(request);
    } else if(uri == m_delUri) {
        delProxy(request);
    } else {
        request.sendNotFound();
    }
    request.sendNotFound();
}


void ProxyApi::listProxy(server::ServerRequest & request) {
    m_service.handleProxyListMessage(request);
}

void ProxyApi::addProxy(server::ServerRequest & request) {
    m_service.handleProxyAddMessage(request);
}

void ProxyApi::delProxy(server::ServerRequest & request) {
    m_service.handleProxyDelMessage(request);
}