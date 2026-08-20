#pragma once


#include <libserver/ServerRequest.h>
#include <libcommon/Common.h>

namespace fs 
{
namespace server 
{
    
static const std::string defaultId = "default";

class HttpHandler
{
public:
    HttpHandler() = default;

    virtual ~HttpHandler(){};

    virtual void handleRequest(ServerRequest & request) = 0;

    virtual bool match(server::ServerRequest & request) = 0;
    
    virtual std::vector<std::string> allUris() = 0;

    virtual bool auth(server::ServerRequest & request) = 0;

    void setId(std::string const& id) {m_id = id;};
    std::string const& id() {return m_id;};

private:
    std::string m_id = defaultId;
};
}
}