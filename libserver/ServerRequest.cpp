#include "ServerRequest.h"

using namespace fs::server;

static const std::regex pattern0(R"(^https?://[^/?#]+((?:/[^?#]*)?(?:\?[^#]*)?))",std::regex::icase);

static const std::regex pattern(R"(^https?://[^/?:#]+(?::\d+)?(.*)$)", std::regex::ECMAScript);

ServerRequest::ServerRequest(HttpRequest *req, HttpResponse *res) {
    m_isValid = true;
    m_headerSended = false;
    m_req = req;
    m_res = res;
    
    m_uri = std::string(http_request_get_uri(req));
    m_method = std::string(http_request_get_method(req));
    m_contentType = getHeader("content-type");
    m_contentLength = http_request_get_content_length(m_req);

    m_referer = getHeader("referer");
    if(m_referer.empty()) {
        m_hasReferer = false;
    } else {
        m_hasReferer = true;
        if(m_referer.find_last_of('?') != std::string::npos) {
            m_referer = "";
            m_hasReferer = false;
        } else {
            std::smatch match;
            if (std::regex_match(m_referer, match, pattern) && match.size() > 1) {
                m_referer = match[1].str();
            } else {
                m_referer = "";
            }
        }
    }
}

std::string ServerRequest::getUriWithQuery() {
    size_t size = http_request_queries_size(m_req);
    std::string uri = m_uri;
    if(size > 0) {
        uri += '?';
    }
    for(size_t i = 0; i < size; i++) {
        uri += common::urlencode(std::string(http_request_queries_get_key(m_req, i))) + "=" + common::urlencode(std::string(http_request_queries_get_val(m_req, i)))+"&";
    }
    if(size > 0) {
        uri = uri.substr(0, uri.length() - 1);
    }
    return uri;
}

std::string ServerRequest::getHeader(std::string const& key) {
    const char *val = http_request_get_header(m_req, key.c_str());
    if(val) {
        return std::string(val);
    }
    return std::string();
}


unsigned int ServerRequest::getHeaderSize() {
    return http_request_headers_size(m_req);
}

std::string ServerRequest::getHeaderKey(unsigned int index) {
    const char *val = http_request_headers_get_key(m_req, index);
    if(val) {
        return std::string(val);
    }
    return std::string();
}

std::string ServerRequest::getHeaderVal(unsigned int index) {
    const char *s = http_request_headers_get_val(m_req, index);
    if(s) {
        return std::string(s);
    }
    return std::string();
}

std::string ServerRequest::getQuery(std::string const& key) {
    const char *s = http_request_get_query(m_req, key.c_str());
    if(s) {
        return std::string(s);
    }
    return std::string();
}

std::string ServerRequest::getAuthKey() {
    return "@@@@####xuyishizhegeshijieshangzuishuaidechaojidashuaige####@@@@";
}

void ServerRequest::sendBadRequest() {
    sendBadRequest("{\"success\":false,\"error\":\"400 BadRequest\"}");
}

void ServerRequest::sendBadRequest(std::string const& reason) {
    doReply(400, reason.c_str());
}

void ServerRequest::sendAuthFailed() {
    doReply(401, "{\"success\":false,\"error\":\"401 NotAuthenticated\"}");
}

void ServerRequest::sendInternalError() {
    doReply(500, "{\"success\":false,\"error\":\"500 InternalError\"}");
}

void ServerRequest::sendServiceUnavailable() {
    doReply(503, "{\"success\":false,\"error\":\"503 ServiceUnavailable\"}");
}

void ServerRequest::sendOk(std::string const& body) {
    doReply(200, body);
}


void ServerRequest::sendRaw(const char *data, const size_t size) {
    m_headerSended = true;
    http_response_set_status(m_res, 200);
    http_response_send_response(m_res, (void *)data, size);
}

void ServerRequest::sendFile(int fd, const size_t size) {
    m_headerSended = true;
    http_response_set_status(m_res, 200);
    http_response_send_file(m_res, fd, size);
}

void ServerRequest::sendNotFound() {
    doReply(404, "{\"success\":false,\"error\":\"404 NotFound\"}");
}


void ServerRequest::doReply(int code, std::string const& body) {
    m_headerSended = true;
    http_response_set_status(m_res, code);
    http_response_set_content_type(m_res, "application/json");
    http_response_send_response(m_res, (const void *)body.c_str(), body.length());
}


void ServerRequest::setResponseHeader(const char *key, const char *value) {
    http_response_set_header(m_res, key, value);
}


void ServerRequest::beginResponse() {
    beginResponse(200);
}

void ServerRequest::beginResponse(int code) {
    m_headerSended = true;
    http_response_set_status(m_res, 200);
    http_response_send_head(m_res);
}

void ServerRequest::sendBytes(const char *data, size_t dataLen) {
    http_response_send_some(m_res, (void *)data, dataLen);
}

void ServerRequest::endResponse() {
    http_response_send_end(m_res);
}


int ServerRequest::readBytes(char *data, size_t dataLen) {
    return http_request_read_some(m_req, (void *)data, dataLen);
}

std::string ServerRequest::readBody() {
    char *data = NULL;
    size_t len = 0;
    http_request_read_all_body(m_req, (void **)&data, &len);
    if(data) {
        std::string body(data, len);
        free(data);
        return body;
    }
    return "";
}




