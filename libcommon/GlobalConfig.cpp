#include "GlobalConfig.h"

using namespace fs::common;

// static GlobalConfig *globalConfigInstance = static_cast<GlobalConfig *>(std::malloc(sizeof(GlobalConfig)));


static GlobalConfig globalConfigInstance = GlobalConfig();

GlobalConfig* GlobalConfig::instance() {
    return &globalConfigInstance;
}

GlobalConfig::GlobalConfig() {

    m_curPath = fs::common::getCurrentPath();

    m_logPath = "logs";
    m_logLevel = LOG_LEVEL_INFO;
    m_logLevelStr = "INFO";

    m_dbPath = "/opt/archer-file/database/";
    m_dbReaders = 4;
    m_dbMemory = 1024 * 1024 * 128;

    m_httpServerAddress = "0.0.0.0";
    m_httpServerPort = 9607;
    m_httpServerThreadNum = 0;
    m_httpServerEnable = true;
    m_httpServerEnableSsl = false;

    m_fileServerStaticDir = "/opt/archer-file/static/";
    m_fileServerEnable = true;
    m_fileServerKey = "fileServerKey@^^";

    m_proxyServerEnable = true;
    m_proxyServerKey = "proxyServerKey@^";

    m_jaServerKey = "rsServerKey@^^^^";
    m_jaServerAddress = "127.0.0.1";
    m_jaServerPort = 9617;
    m_jaThreadNum = 0;

    m_jaServerEnable = true;
}

void GlobalConfig::parseConfig(std::string const & configPath) {
    std::string abConfigPath = configPath;
    if(!isAbsolutePath(configPath)) {
        abConfigPath = m_curPath + "/" + configPath;
    }
    if(fileExists(abConfigPath)) {
        console_out("Using config file %s", configPath.c_str());

        std::ifstream file(configPath);
        if(!file.is_open()) {
            console_err("Can not open file %s", configPath.c_str());
            exit(0);
        }
        Json::CharReaderBuilder builder;
        std::string errs;

        if (!Json::parseFromStream(builder, file, &m_root, &errs)) {
            console_err("Parse json file %s failed, due to %s", configPath.c_str(), errs.c_str());
            file.close();
            exit(0);
        }
        file.close();

        console_out("Parse log configs");
        if(m_root.isMember("log")) {
            if(m_root["log"].isMember("path")) {
                m_logPath = std::string(m_root["log"]["path"].asCString());
                if(m_logPath.empty()) {
                    m_logPath = "logs";
                }
            } else {
                m_logPath = "logs";
            }

            if(m_root["log"].isMember("level")) {
                m_logLevelStr = std::string(m_root["log"]["level"].asCString());
                if(m_logLevelStr == "TRACE") {
                    m_logLevel = LOG_LEVEL_TRACE;
                } else if(m_logLevelStr == "DEBUG") {
                    m_logLevel = LOG_LEVEL_DEBUG;
                } else if(m_logLevelStr == "INFO") {
                    m_logLevel = LOG_LEVEL_INFO;
                } else if(m_logLevelStr == "WARN") {
                    m_logLevel = LOG_LEVEL_WARN;
                } else if(m_logLevelStr == "ERROR") {
                    m_logLevel = LOG_LEVEL_ERROR;
                } else if(m_logLevelStr == "FATAL") {
                    m_logLevel = LOG_LEVEL_FATAL;
                } else {
                    m_logLevelStr = "INFO";
                    m_logLevel = LOG_LEVEL_INFO;
                }
            } else {
                m_logLevel = LOG_LEVEL_INFO;
            }
        } else {
            m_logPath = "logs";
            m_logLevel = LOG_LEVEL_INFO;
        }
        
        if(m_root.isMember("http")) {
            Json::Value http = m_root["http"];
            if(http.isMember("enable")) {
                m_httpServerEnable = http["enable"].asBool();
            }
            if(http.isMember("host")) {
                m_httpServerAddress = http["host"].asString();
            }
            if(http.isMember("port")) {
                m_httpServerPort = http["port"].asUInt();
            }
            if(http.isMember("threadNum")) {
                m_httpServerThreadNum = http["threadNum"].asUInt();
            }
            if(http.isMember("enableSsl")) {
                m_httpServerEnableSsl = http["enableSsl"].asBool();
            }

            if(m_httpServerEnableSsl) {
                if(!http.isMember("crtPath")) {
                    console_err("http.crtPath is required once ssl is enabled");
                    exit(0);
                } else {
                    m_httpServerCrtPath = http["crtPath"].asString();
                }
                if(!http.isMember("keyPath")) {
                    console_err("http.keyPath is required once ssl is enabled");
                    exit(0);
                } else {
                    m_httpServerKeyPath = http["keyPath"].asString();
                }
                if(http.isMember("enCrtPath")) {
                    m_httpServerEnCrtPath = http["enCrtPath"].asString();
                }
                if(http.isMember("enKeyPath")) {
                    m_httpServerEnKeyPath = http["enKeyPath"].asString();
                }
            }
            
            if(http.isMember("fileServer")) {
                Json::Value fileServer = http["fileServer"];
                if(fileServer.isMember("enable")) {
                    m_fileServerEnable = fileServer["enable"].asBool();
                }
                if(fileServer.isMember("staticDir")) {
                    m_fileServerStaticDir = fileServer["staticDir"].asString();
                }
                if(fileServer.isMember("authKey")) {
                    m_fileServerKey = fileServer["authKey"].asString();
                }
                if(m_fileServerStaticDir.empty()) {
                    m_fileServerStaticDir = "/opt/archer-file/static/";
                } else if(m_fileServerStaticDir[m_fileServerStaticDir.length() - 1] != '/') {
                    m_fileServerStaticDir += "/";
                }
            }
            
            if(http.isMember("proxyServer")) {
                Json::Value proxyServer = http["proxyServer"];
                if(proxyServer.isMember("enable")) {
                    m_proxyServerEnable = proxyServer["enable"].asBool();
                }
                if(proxyServer.isMember("authKey")) {
                    m_proxyServerKey = proxyServer["authKey"].asString();
                }
            }
        }
        if(m_root.isMember("remoteStorageServer")) {
            Json::Value ja = m_root["remoteStorageServer"];
            if(ja.isMember("enable")) {
                m_jaServerEnable = ja["enable"].asBool();
            }
            if(ja.isMember("host")) {
                m_jaServerAddress = ja["host"].asString();
            }
            if(ja.isMember("port")) {
                m_jaServerPort = ja["port"].asUInt();
            }
            if(ja.isMember("authKey")) {
                m_jaServerKey = ja["authKey"].asString();
            }
            if(ja.isMember("threadNum")) {
                m_jaThreadNum = ja["threadNum"].asUInt();
            }
        }

        
        if(m_root.isMember("database")) {
            Json::Value database = m_root["database"];
            if(database.isMember("path")) {
                m_dbPath = database["path"].asString();
            }
            if(database.isMember("readers")) {
                m_dbReaders = database["readers"].asUInt();
            }
            if(database.isMember("memory")) {
                m_dbMemory = database["memory"].asUInt();
            }
            if(m_dbPath.empty()) {
                m_dbPath = "/opt/archer-file/database/";
            } else if(m_dbPath[m_dbPath.length() - 1] != '/') {
                m_dbPath += "/";
            }
        }
    } else {
        console_out("Default file server authKey = %s", m_fileServerKey.c_str());
        console_out("Default proxy server authKey = %s", m_proxyServerKey.c_str());
        console_out("Default remote storage server authKey = %s", m_jaServerKey.c_str());
    }

    Logger::instance().setLevel(m_logLevel);
    Logger::instance().setLogPath(m_logPath);

    if(!fileExists(m_fileServerStaticDir)) {
        createDirectories(m_fileServerStaticDir);
    }
    if(!fileExists(m_dbPath)) {
        createDirectories(m_dbPath);
    }
    
    console_out("Log path = %s", m_logPath.c_str());
    console_out("Log level = %s", m_logLevelStr.c_str());
    console_out("Database readers = %d", m_dbReaders);
    console_out("Database memory size = %u", m_dbMemory);
}

