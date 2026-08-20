#pragma once


#include <libcommon/GlobalConfig.h>
#include <libhandler/JaHandler.h>
#include <libhandler/HttpHandler.h>

#include "JaContext.h"

#include "archer_net.h"


#ifndef JA_MAGIC
#define JA_MAGIC                  "9607"
#endif

// 1  ~  32   for   errors
#define ERROR_HEAD_TYPE     1
// 33 ~  64   for   client
#define CLIENT_GET_TYPE     33
#define CLIENT_SAVE_TYPE    34
// 65 ~  96   for   server
#define SERVER_OK_TYPE      65
#define SERVER_FAIL_TYPE    66


namespace fs 
{
namespace server 
{

class JaServer
{
public:

    JaServer(fs::handler::JaHandler &handler);
    ~JaServer();

    void listen(std::string const& host, std::uint16_t const& port);

    void close();

    void useMultiThreads(uint16_t const& threadNums);

    void handle(Channel *channel, char *data, size_t data_len);

    void setKey(std::string const& key) {
        m_key = key;
    }
 
private:

    ServerChannel                                *m_server;
    BaseHandler                                  *m_baseHandler;
    uint16_t                                      m_threadNum = 0;

    std::string                                   m_key;
    fs::handler::JaHandler                       &m_handler;
};
}
}

