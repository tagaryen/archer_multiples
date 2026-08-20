#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libdatabase/DataBase.h>
#include <libserver/JaContext.h>

#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "archer_net.h"

namespace fs 
{
namespace handler 
{
class JaHandler
{
public:

    JaHandler(database::DataBase & database) : m_database(database)  {};
    ~JaHandler() = default;
    void handleMessage(server::JaContext &ctx, char *data, size_t data_len);
private:
    database::DataBase & m_database;
};
}
}

