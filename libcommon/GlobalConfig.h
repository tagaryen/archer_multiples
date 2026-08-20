#pragma once

#include <fstream>
#include <sstream>
#include <json/json.h>

#include "Common.h"
#include "Logger.h"


namespace fs 
{
namespace common 
{

class GlobalConfig 
{
public:
    
    static GlobalConfig* instance();

    GlobalConfig();
    ~GlobalConfig() = default;

    void parseConfig(std::string const & configPath);
    
    std::string const& fetchCurPath()  {return m_curPath;}

    std::string const& fetchLogPath()  {return m_logPath;}
    uint16_t fetchLogLevel()  {return m_logLevel;}
    
    bool httpServerEnabled() {return m_httpServerEnable;}
    std::string const& fetchHTTPServerAddress()  {return m_httpServerAddress;}
    uint16_t fetchHTTPServerPort()  {return m_httpServerPort;}
    uint16_t fetchHTTPThreadNum()  {return m_httpServerThreadNum;}
    bool httpServerEnabledSsl() {return m_httpServerEnableSsl;}
    std::string const& fetchHTTPServerCrtPath()  {return m_httpServerCrtPath;}
    std::string const& fetchHTTPServerKeyPath()  {return m_httpServerKeyPath;}
    std::string const& fetchHTTPServerEnCrtPath()  {return m_httpServerEnCrtPath;}
    std::string const& fetchHTTPServerEnKeyPath()  {return m_httpServerEnKeyPath;}

    bool fileServerEnabled() {return m_fileServerEnable;};
    std::string const& fetchFileStaticDir() {return m_fileServerStaticDir;}
    std::string const& fetchFileServerKey() {return m_fileServerKey;}

    bool proxyServerEnabled() {return m_proxyServerEnable;};
    std::string const& fetchProxyServerKey() {return m_proxyServerKey;}

    std::string const& fetchDatabasePath()  {return m_dbPath;}
    uint16_t fetchDatabaseReaders()  {return m_dbReaders;}
    uint32_t fetchDatabaseMemory()  {return m_dbMemory;}
    
    bool jaServerEnabled() {return m_fileServerEnable;};
    std::string const& fetchRSServerAddress()  {return m_jaServerAddress;}
    std::string const& fetchRSServerKey()  {return m_jaServerKey;}
    uint16_t fetchRSServerPort()  {return m_jaServerPort;}
    uint16_t fetchRSThreadNum()  {return m_jaThreadNum;}

private:

    std::string m_curPath;

    std::string m_logPath;
    uint16_t    m_logLevel;
    std::string m_logLevelStr;

    std::string m_dbPath;
    uint16_t    m_dbReaders;
    uint32_t    m_dbMemory;

    std::string m_httpServerAddress;
    uint16_t    m_httpServerPort;
    uint16_t    m_httpServerThreadNum;
    bool        m_httpServerEnable;
    bool        m_httpServerEnableSsl;
    std::string m_httpServerCrtPath;
    std::string m_httpServerKeyPath;
    std::string m_httpServerEnCrtPath;
    std::string m_httpServerEnKeyPath;

    std::string m_fileServerStaticDir;
    std::string m_fileServerKey;
    bool        m_fileServerEnable;

    bool        m_proxyServerEnable;
    std::string m_proxyServerKey;

    bool        m_jaServerEnable;
    std::string m_jaServerAddress;
    uint16_t    m_jaServerPort;
    std::string m_jaServerKey;
    uint16_t    m_jaThreadNum;

    Json::Value m_root;
};
}
}