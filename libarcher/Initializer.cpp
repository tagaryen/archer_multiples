#include "Initializer.h"


void fs::initializer::startRemoteStoreServer(fs::database::DataBase & database) {
    std::thread jaThread([&database]{ 
        fs::common::GlobalConfig *config = fs::common::GlobalConfig::instance();
        
        std::string address = config->fetchRSServerAddress();
        uint16_t port = config->fetchRSServerPort();
        uint16_t threadNum = config->fetchRSThreadNum();
        
        fs::handler::JaHandler handler(database);
        fs::server::JaServer server(handler);
        server.setKey(config->fetchRSServerKey());
        server.useMultiThreads(threadNum);
        server.listen(address, port);

    });
    jaThread.detach();
}

void fs::initializer::startHTTPServer(fs::database::DataBase & database) {
    fs::common::GlobalConfig *config = fs::common::GlobalConfig::instance();

    std::string address = config->fetchHTTPServerAddress();
    uint16_t port = config->fetchHTTPServerPort();
    uint16_t threadNum = config->fetchHTTPThreadNum();

    fs::server::ArcherServer server;
    server.useMultiThreads(threadNum);

    fs::service::FileService fileService(database);
    std::shared_ptr<fs::api::FileApi> fileApi = std::make_shared<fs::api::FileApi>(fileService, config->fetchFileServerKey());
    if(config->fileServerEnabled()) {
        server.addHandler(fileApi);
    }
    
    fs::service::ProxyService proxyService(database, server);
    std::shared_ptr<fs::api::ProxyApi> proxyApi = std::make_shared<fs::api::ProxyApi>(proxyService, config->fetchProxyServerKey());
    if(config->proxyServerEnabled()) {
        server.addHandler(proxyApi);
        for(fs::database::ProxyInfo proxy: database.listProxy()) {
            std::shared_ptr<fs::handler::ProxyHandler> proxyHandler = std::make_shared<fs::handler::ProxyHandler>(proxy.getConfig());
            server.addProxyHandler(proxyHandler);
        }
    }

    server.listen(address, port);
}

void fs::initializer::init() {
    std::string configPath = "config.json";
    fs::common::GlobalConfig *config = fs::common::GlobalConfig::instance();
    config->parseConfig(configPath);

    std::string databasePath = config->fetchDatabasePath();
    uint16_t readers = config->fetchDatabaseReaders();
    uint32_t memory = config->fetchDatabaseMemory();
    fs::database::DataBase database(databasePath, readers, memory);
    
    if(config->jaServerEnabled()) {
        startRemoteStoreServer(database);
    }

    if(config->httpServerEnabled()) {
        startHTTPServer(database);
    }
}
