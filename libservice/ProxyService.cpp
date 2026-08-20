#include "ProxyService.h"

#include <stdio.h>
#include <sys/file.h>
#include <sstream>  
#include <regex>  

using namespace fs::service;

void ProxyService::handleProxyListMessage(server::ServerRequest & request) {
    std::vector<fs::database::ProxyInfo> list = m_database.listProxy();
    std::string resultJson = "[";
    std::string size;
    for(auto p: list) {
        resultJson += "{\"name\":\"" + p.getName() + "\",\"time\":\""+p.getUpdateTime()+"\",\"config\":"+p.getConfig()+"},";
    }
    if(!list.empty()) {
        resultJson = resultJson.substr(0, resultJson.length() - 1);
    }
    resultJson += "]";
    request.sendOk("{\"success\":true, \"data\":" + resultJson + "}");
}

void ProxyService::handleProxyAddMessage(server::ServerRequest & request) {
    std::string body = request.readBody();
    if(body.empty()) {
        request.sendBadRequest("Body Error");
        return ;
    }
    std::shared_ptr<handler::ProxyHandler> handler = std::make_shared<handler::ProxyHandler>(body);
    if(!handler->isValid()) {
        LOG_warn("ProxyService invalid body config %s", body.c_str());
        request.sendOk("{\"success\":false, \"message\":\"param error\"}");
        return ;
    }
    database::ProxyInfo info(handler->getRequestPath(), handler->getRaw());
    if(m_server.addProxyHandler(handler)) {
        m_database.saveProxy(info);
        request.sendOk("{\"success\":true}");
    } else {
        request.sendOk("{\"success\":false, \"message\":\"duplicated request path\"}");
    }
}

void ProxyService::handleProxyDelMessage(server::ServerRequest & request) {
    std::string name = request.getQuery("name");
    LOG_info("Proxy delete -name=%s", name.c_str());
    m_server.removeProxyHandler(name);
    m_database.delProxy(name);
    request.sendOk("{\"success\":true}");
}


