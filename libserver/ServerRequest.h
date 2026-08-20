#pragma once

#include "archer_alg.h"
#include "archer_net.h"

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>

namespace fs
{
namespace server
{
class ServerRequest
{
public:
    ServerRequest(HttpRequest *req, HttpResponse *res);
    ~ServerRequest(){};

    void sendBadRequest();
    void sendBadRequest(std::string const& reason);
    void sendAuthFailed();
    void sendInternalError();
    void sendNotFound();
    void sendServiceUnavailable();

    void sendOk(std::string const& body);
    void sendRaw(const char *data, const size_t size);
    void sendFile(int fd, const size_t size);
    void beginResponse();
    void beginResponse(int code);
    void sendBytes(const char *data, size_t dataLen);
    void endResponse();
    int readBytes(char *data, size_t dataLen);
    std::string readBody();

    bool headerSended() {return m_headerSended;}
    bool hasReferer() {return m_hasReferer;}
    bool isValid() {return m_isValid;}
    std::string const& getMethod() {return m_method;}
    std::string const& getUri() {return m_uri;}
    std::string const& setUri(std::string const& uri) {m_uri = uri;}
    std::string getUriWithQuery();
    uint32_t getContentLength() {return m_contentLength;}
    std::string const& getContentType() {return m_contentType;}
    std::string const& getReferer() {return m_referer;}
    std::string getHeader(std::string const& key);
    unsigned int getHeaderSize();
    std::string getHeaderKey(unsigned int index);
    std::string getHeaderVal(unsigned int index);
    std::string getQuery(std::string const& key);
    std::string getAuthKey();
    void setResponseHeader(const char *key, const char *value);

private:
    void doReply(int code, std::string const& body);

private:
    bool                            m_headerSended;

    bool                            m_isValid;
    bool                            m_hasReferer;
    std::string                     m_referer;

    std::string                     m_method;
    std::string                     m_uri;
    uint32_t                        m_contentLength;
    std::string                     m_contentType;

    HttpRequest                    *m_req;
    HttpResponse                   *m_res;
};

}
}