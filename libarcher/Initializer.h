#pragma once

// #include <libcommon/Log.h>
#include <libcommon/GlobalConfig.h>
#include <libapi/FileApi.h>
#include <libapi/ProxyApi.h>
#include <libdatabase/DataBase.h>
#include <libservice/FileService.h>
#include <libservice/ProxyService.h>
#include <libhandler/JaHandler.h>
#include <libserver/ArcherServer.h>
#include <libserver/JaServer.h>


namespace fs
{
namespace initializer 
{

void startRemoteStoreServer(fs::database::DataBase & database);

void startHTTPServer(fs::database::DataBase & database);

void init();

}
}