#pragma once


#include <libcommon/GlobalConfig.h>

#include "archer_net.h"


#ifndef JA_MAGIC
#define JA_MAGIC                  "9607"
#endif

// 1  ~  32   for   errors
#define ERROR_HEAD_TYPE     1
#define ERROR_TYPE_TYPE     2
#define ERROR_BODY_TYPE     3
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

class JaContext 
{
public:
    JaContext(Channel *channel) {
        m_channel = channel;
    };
    ~JaContext() = default;

    void sendSuccess();
    void sendSuccess(std::string const& key, std::string const& value);
    void sendError(char type);
    int checkHeader(char *data, size_t data_len, const char *key);

private:

    Channel *m_channel;

    const char *m_header = NULL;

};
}
}

