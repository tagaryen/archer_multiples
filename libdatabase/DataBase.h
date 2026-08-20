#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include "lmdb.h"

#include <mutex>
#include <array>
#include <unordered_map>

namespace fs 
{
namespace database 
{

class FileInfo 
{
public:

    static std::string genName(std::string const& realName, std::string const& key);

    FileInfo() {m_valid = false;}
    FileInfo(std::string const& realName, std::string const& key);
    ~FileInfo(){}

    bool isValid() {return m_valid;}
    std::string const& getName() {return m_name;}
    std::string const& getRealName() {return m_realName;}
    std::string const& getKey() {return m_key;}
    std::string const& getUpdateTime() {return m_updateTime;}
    size_t getSize() {return m_size;};
    std::string const& getContentType() {return m_contentType;}
    void setUpdateTime(std::string const& updateTime) {m_updateTime=updateTime;}
    void setSize(size_t size) {m_size = size;};
    void setContentType(std::string const& contentType) {m_contentType = contentType;};
    
    std::string encodeToListInfo();
    void decodeFromListInfo(std::string const& encoded, std::string const& key);

    std::string encode();
    void decode(const char *data, size_t data_len);


private:
    bool             m_valid;
    
    std::string      m_name;
    std::string      m_realName;
    std::string      m_key;
    std::string      m_updateTime;
    std::string      m_contentType;
    size_t           m_size;
};

class Page {

public:
    Page(int total, int pageNum, std::vector<FileInfo> const& files):m_total(total), m_pageNum(pageNum), m_files(std::move(files)) {}
    int getTotal() {return m_total;}
    int getPageNum() {return m_pageNum;}
    std::vector<FileInfo> const& getFile() {return m_files;}
private:
    int m_total;
    int m_pageNum;
    std::vector<FileInfo> m_files;
};


class ProxyInfo 
{
public:

    ProxyInfo(std::string const& requestPath, std::string const& config):m_config(config) {
        if(requestPath.empty()) {
            m_name = "/";
        } else { 
            m_name = requestPath;
            if(requestPath[requestPath.length() - 1] != '/') {
                m_name += '/';
            }
        }
        m_name = common::base64Encode(m_name);
        m_updateTime = fs::common::getNowTime();
    }
    ProxyInfo() {}
    ~ProxyInfo(){}

    std::string const& getName() {return m_name;}
    std::string const& getUpdateTime() {return m_updateTime;}
    std::string const& getConfig() {return m_config;}
    
    std::string encode();
    size_t decode(const char *data, size_t data_len);

private:

    std::string      m_name;
    std::string      m_updateTime;
    std::string      m_config;
};

class DataBase 
{

public:
    DataBase(std::string const& dbPath, unsigned int readerNum, size_t maxMemorySize);
    
    ~DataBase() {
        mdb_env_close(m_env);
    }

    Page listFile(std::string const& key, int pageNum);
    int saveFileInfo(FileInfo & fileInfo);
    void getFileInfo(FileInfo & fileInfo);

    int saveJsonInfo(std::string const& key, std::string const& val);
    std::string getJsonInfo(std::string const& key);

    int saveProxy(ProxyInfo & proxyInfo);
    std::vector<ProxyInfo> listProxy();
    int delProxy(std::string const& name);
private:

    bool doError(int rc) {
        if(rc) {
            char *msg = mdb_strerror(rc);
            console_err("%s", msg);
            LOG_error("%s", msg);
        }
        return rc;
    }

    int saveFileList(FileInfo & fileInfo);
    std::vector<FileInfo> getFileList(std::string const& key);
    std::string getFileListStr(std::string const& key);
    std::string getProxyListStr();

    void clearOldKeys();

    std::unordered_map<std::string,std::string>  m_cache;
    std::mutex                           m_mutex;

    MDB_env    *m_env;
    MDB_dbi     m_dbi;
};

}

}
