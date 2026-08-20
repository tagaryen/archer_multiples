#include "JaServer.h"

using namespace fs::server;

static void server_on_connect_cb(Channel *channel) {
    LOG_info("Remote %s:%d connected", channel_get_host(channel), channel_get_port(channel));
}

static void channel_on_close_cb(Channel *channel) {
    LOG_warn("Remote %s:%d closed", channel_get_host(channel), channel_get_port(channel));
}

static void channel_on_read_cb(Channel *channel, char *data, size_t data_len) {
    JaServer *server = (JaServer *) channel_get_arg(channel);
    server->handle(channel, data, data_len);
}

static void channel_on_error_cb(Channel *channel, const char *err_msg) {
    LOG_error("Remote %s:%d error, %s", channel_get_host(channel), channel_get_port(channel), err_msg);
}


JaServer::JaServer(fs::handler::JaHandler &handler):m_handler(handler) {
    m_server = server_channel_new();
    m_baseHandler = base_handler_new();
    server_channel_set_arg(m_server, this);
    
    base_handler_set_channel_on_connect(m_baseHandler, server_on_connect_cb);
    base_handler_set_channel_on_read(m_baseHandler, channel_on_read_cb);
    base_handler_set_channel_on_error(m_baseHandler, channel_on_error_cb);
    base_handler_set_channel_on_close(m_baseHandler, channel_on_close_cb);
    base_handler_handle_server_channel(m_baseHandler, m_server);
}

JaServer::~JaServer() {
    server_channel_close(m_server);
    server_channel_free(m_server);
    base_handler_free(m_baseHandler);
}


void JaServer::listen(std::string const& host, std::uint16_t const& port) {
    if(m_threadNum > 0) {
        server_channel_set_read_threads(m_server, m_threadNum);
    }

    console_out("RomteStorage Server listenning on %s:%d", host.c_str(), port);
    LOG_info("RomteStorage Server listenned on %s:%d", host.c_str(), port);
    if(!server_channel_listen(m_server, host.c_str(), port)) {
        const char *errstr = server_channel_get_errstr(m_server);
        console_err("RomteStorage Server listen on %s:%d error, %s", host.c_str(), port, errstr);
        LOG_error("RomteStorage Server listen on %s:%d error, %s", host.c_str(), port, errstr);
    }
}

void JaServer::close() {
    server_channel_close(m_server);
}

void JaServer::useMultiThreads(uint16_t const& threadNum) {
    m_threadNum = threadNum;
}

void JaServer::handle(Channel *channel, char *data, size_t data_len) {
    JaContext ctx(channel);
    if(ctx.checkHeader(data, data_len, m_key.c_str()) < 0) {
        LOG_warn("RomteStorage server check head failed");
        channel_close(channel);
        return ;
    }
    m_handler.handleMessage(ctx, data + 52, data_len - 52);
}
