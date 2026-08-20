#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libserver/ArcherServer.h>
#include <libserver/ServerRequest.h>
#include <libhandler/ProxyHandler.h>
#include <libdatabase/DataBase.h>

namespace fs 
{
namespace service 
{
class ProxyService
{
public:

    ProxyService(database::DataBase & database, server::ArcherServer & server) :m_database(database), m_server(server) {}

    ~ProxyService() {};

    void handleProxyListMessage(server::ServerRequest & request);

    void handleProxyAddMessage(server::ServerRequest & request);
    
    void handleProxyDelMessage(server::ServerRequest & request);

private:
    database::DataBase & m_database;
    server::ArcherServer & m_server;
};
}
}