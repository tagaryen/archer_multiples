#include "DataBase.h"
#include <regex>

using namespace fs::database;

const std::string proxyListKey = "@@@@@@@@xuyishizhegeshijieshangzuishuaidechaojidashuaige@@@@@@@@";

static char* fileInfoGetPrintStr(const char *data, size_t data_len) {
    char *printStr = (char *)malloc(data_len + 1);
    memcpy(printStr, data, data_len);
    printStr[data_len] = '\0';
    return printStr;
}


std::string FileInfo::genName(std::string const& realName, std::string const& key) {
    std::string name = realName + key;
    Hash32 hash;
    sm3((uint8_t *)name.c_str(), name.length(), &hash);
    return common::uint8sToHex(hash.h, 32);
}

FileInfo::FileInfo(std::string const& realName, std::string const& key) {
    m_valid = true;
    std::string name = realName + key;
    Hash32 hash;
    sm3((uint8_t *)name.c_str(), name.length(), &hash);
    m_name = common::uint8sToHex(hash.h, 32);
    m_realName = realName;
    m_key = key;
    m_updateTime = common::getNowTime();
    m_size = 0;
    m_contentType = "application/none";
    
    LOG_trace("Fileinfo save name = %s", m_name.c_str());
}

// 64 + 19 + 8 + len(name)
std::string FileInfo::encodeToListInfo() {
    std::string sizeHex = common::sizeToHex(m_size);
    return m_name + m_updateTime + sizeHex + m_realName;
}

// 64 + 19 + 8 + len(name)
void FileInfo::decodeFromListInfo(std::string const& encoded, std::string const& key) {
    m_valid = false;
    LOG_trace("Fileinfo decode list info = %s", encoded.c_str());
    if(encoded.length() < 64 + 19 + 8) {
        LOG_error("fileInfo Can not parse database value to FileInfo, Value = %s", encoded.c_str());
        return ;
    }
    
    m_key = key;
    m_name = encoded.substr(0, 64);
    m_updateTime = encoded.substr(64, 19);
    std::string sizeHex = encoded.substr(64 + 19, 8);
    m_size = common::hexToSize(sizeHex);
    m_realName = encoded.substr(64 + 19 + 8);
    m_valid = true;
}

// 64 + 32 + 19 + 8 + 8 + len(name) + len(contentType)
std::string FileInfo::encode() {
    std::string nameLen = common::sizeToHex(m_realName.length());
    std::string sizeHex = common::sizeToHex(m_size);
    
    printf("encoded=%s\n", (m_name + m_key + m_updateTime + sizeHex + nameLen + m_realName + m_contentType).c_str());
    fflush(stdout);
    return m_name + m_key + m_updateTime + sizeHex + nameLen + m_realName + m_contentType;
}

// 64 + 32 + 19 + 8 + 8 + len(name) + len(contentType)
void FileInfo::decode(const char *data, size_t dataLen) {
    LOG_trace("Fileinfo raw size = %llu", dataLen);
    
    m_valid = false;

    if(dataLen < 64 + 64 + 19 + 8 + 8 + 4) {
        char *printStr = fileInfoGetPrintStr(data, dataLen);
        LOG_error("fileInfo Can not parse database value to FileInfo, Value = %s", printStr);
        free(printStr);
        return ;
    }

    std::string encoded = std::string(data, dataLen);
    m_name = encoded.substr(0, 64);
    m_key = encoded.substr(64, 64);
    m_updateTime = encoded.substr(64 + 64, 19);
    std::string sizeHex = encoded.substr(64 + 64 + 19, 8);
    m_size = common::hexToSize(sizeHex);
    std::string nameLenHex = encoded.substr(64 + 64 + 19 + 8, 8);
    size_t nameLen = common::hexToSize(nameLenHex);
    std::string nameAndType = encoded.substr(64 + 64 + 19 + 8 + 8);
    if(nameAndType.length() < nameLen + 4) {
        char *printStr = fileInfoGetPrintStr(data, dataLen);
        LOG_error("fileInfo Can not parse database value to FileInfo, Value = %s", printStr);
        free(printStr);
        return ;
    }
    m_realName = nameAndType.substr(0, nameLen);
    m_contentType = nameAndType.substr(nameLen);
    m_valid = true;
}

// 8 + len(name) + 19 + 8 + len(config)
std::string ProxyInfo::encode() {
    std::string nameLen = common::sizeToHex(m_name.length());
    std::string configLen = common::sizeToHex(m_config.length());
    return nameLen + m_name + m_updateTime + configLen + m_config;
}
// 8 + len(name) + 19 + 8 + len(config)
size_t ProxyInfo::decode(const char *data, size_t data_len) {
    if(data_len < 8 + 19 + 8 + 3) {
        return 0;
    }
    size_t off = 0;
    size_t nameLen = common::hexToSize(std::string(data, 8));
    off += 8;
    m_name = std::string(data + off, nameLen);
    off += nameLen;
    m_updateTime = std::string(data + off, 19);
    off += 19;
    size_t configLen = common::hexToSize(std::string(data + off, 8));
    off += 8;
    m_config = std::string(data + off, configLen);
    off += configLen;
    return off;
}



DataBase::DataBase(std::string const& dbPath, unsigned int readerNum, size_t maxMemorySize){
    if(doError(mdb_env_create(&m_env))) {
        console_err("Can not create lmdb file database environment. Exit(0)\n");
        LOG_error("Can not create lmdb file database environment. Exit(0)\n");
        exit(0);
    }
    if(doError(mdb_env_set_maxdbs(m_env, 4))) {
        console_err("Can not set lmdb file database max db nums. Exit(0)");
        LOG_error("Can not set lmdb file database max db nums. Exit(0)");
        exit(0);
    }
    if(doError(mdb_env_set_maxreaders(m_env, readerNum))) {
        console_err("Can not set lmdb file database readers. Exit(0)");
        LOG_error("Can not set lmdb file database readers. Exit(0)");
        exit(0);
    }
    if(doError(mdb_env_set_mapsize(m_env, maxMemorySize))) {
        console_err("Can not set lmdb file database max memory size. Exit(0)");
        LOG_error("Can not set lmdb file database max memory size. Exit(0)");
        exit(0);
    }

    fs::common::createDirectories(dbPath);
    if(doError(mdb_env_open(m_env, dbPath.c_str(), 0, 0664))) {
        console_err("Can not open lmdb file database dir '%s'. Exit(0)", dbPath.c_str());
        LOG_error("Can not open lmdb file database dir '%s'. Exit(0)", dbPath.c_str());
        exit(0);
    }

    MDB_txn *txn;
    if(doError(mdb_txn_begin(m_env, NULL, 0, &txn))) {
        console_err("Begin init file transaction failed. Exit(0)");
        LOG_error("Begin init file transaction failed. Exit(0)");
        exit(0);
    }

    if(doError(mdb_dbi_open(txn, "fileinfo", MDB_CREATE, &m_dbi))) {
        console_err("Open file database failed. Exit(0)");
        LOG_error("Open file database failed. Exit(0)");
        mdb_txn_abort(txn);
        exit(0);
    }
    mdb_txn_commit(txn);

    console_out("Create lmdb file database success");
    LOG_info("Create lmdb file database success");
}


std::string DataBase::getFileListStr(std::string const& key) {
    int rc = 0;

    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_cursor *cursor = NULL;
    MDB_txn *txn;

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin read file list failed, due to %s", mdb_strerror(rc));
        return "";
    }

    dbKey.mv_size = key.length();
    dbKey.mv_data = (void *)(key.c_str());

    dbValue.mv_size = 0;
    // LOG_trace("database Begin read transaction Key = %s", key.c_str());
    if((rc = mdb_get(txn, m_dbi, &dbKey, &dbValue))) {
        LOG_error("database Readding file list failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return "";
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database read Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return "";
    }
    return std::string((char *)dbValue.mv_data, dbValue.mv_size);
}

std::vector<FileInfo> DataBase::getFileList(std::string const& key) {
    std::string val = getFileListStr(key);
    if(val.empty()) {
        return  std::vector<FileInfo>();
    }
    std::regex reg("@`");
    std::vector<std::string> datas(std::sregex_token_iterator(val.begin(), val.end(), reg, -1), std::sregex_token_iterator());
    std::vector<FileInfo> result;
    for(std::string data: datas) {
        FileInfo fileInfo;
        fileInfo.decodeFromListInfo(data, key);
        if(fileInfo.isValid()) {
            result.emplace_back(fileInfo);
        }
    }
    return result;
}

//time = yyyy-MM-dd HH:mm:ss  len = 19
int DataBase::saveFileList(FileInfo & fileInfo) {
    
    std::string key = fileInfo.getKey();
    std::string value = getFileListStr(key);
    if(!value.empty()) {
        value += "@";
        value += fileInfo.encodeToListInfo();
    } else {
        value = fileInfo.encodeToListInfo();
    }

    int rc = 0;
    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_txn *txn;
    
    LOG_trace("Begin to save file list to database");

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin write transaction write file list failed, due to %s", mdb_strerror(rc));
        return rc;
    }

    LOG_trace("database Begin write file list, value = %s", value.c_str());

    dbKey.mv_size = key.length();
    dbKey.mv_data = (void *)key.c_str();
    dbValue.mv_size = value.length();
    dbValue.mv_data = (void *)value.c_str();

    if((rc = mdb_put(txn, m_dbi, &dbKey, &dbValue, 0))) {
        LOG_error("database Writting data failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }

    LOG_trace("Save file information to database end");

    return rc;
}

Page DataBase::listFile(std::string const& key, int pageNum) {
    if(pageNum <= 0) {
        pageNum = 1;
    }
    std::vector<FileInfo> list = getFileList(key);
    if(list.empty()) {
        return Page(0, pageNum, std::vector<FileInfo>());
    } 
    std::sort(list.begin(), list.end(), [](FileInfo & a, FileInfo & b) {
        return fs::common::timeToLong(a.getUpdateTime()) > fs::common::timeToLong(b.getUpdateTime());
    });
    size_t total = list.size();
    std::vector<FileInfo> files;
    int start = (pageNum - 1) * 10, end = pageNum * 10;
    if(start > list.size()) {
        start = total;
    }
    if(end > total) {
        end = total;
    }
    return Page(total, pageNum, std::vector<FileInfo>(list.begin() + start, list.begin() + end));
}

int DataBase::saveFileInfo(FileInfo & fileInfo) {
    int rc = 0;
    if(rc = saveFileList(fileInfo)) {
        return rc;
    }

    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_txn *txn;

    std::string key = fileInfo.getName();
    std::string value = fileInfo.encode();
    
    LOG_trace("Begin to save file information to database with key = '%s', key.length = %llu", key.c_str(), key.length());

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin write transaction failed, due to %s", mdb_strerror(rc));
        return rc;
    }

    LOG_trace("database Begin write transaction key = %s, Filename = %s", 
            fileInfo.getKey().c_str(), fileInfo.getRealName().c_str());

    dbKey.mv_size = key.length();
    dbKey.mv_data = (void *)key.c_str();
    dbValue.mv_size = value.length();
    dbValue.mv_data = (void *)value.c_str();

    if((rc = mdb_put(txn, m_dbi, &dbKey, &dbValue, 0))) {
        LOG_error("database Writting data failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }

    LOG_trace("Save file information to database with key = '%s' end", key.c_str());

    return rc;
}

void DataBase::getFileInfo(FileInfo & fileInfo) {
    int rc = 0;
    
    std::string name = fileInfo.getName();

    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_cursor *cursor = NULL;
    MDB_txn *txn;

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin read transaction failed, due to %s", mdb_strerror(rc));
        return ;
    }

    LOG_trace("Database get key: %s, key.length:%llu", name.c_str(), name.length());

    dbKey.mv_size = name.length();
    dbKey.mv_data = (void *)(name.c_str());

    dbValue.mv_size = 0;
    // LOG_trace("database Begin read transaction Key = %s", name.c_str());
    if((rc = mdb_get(txn, m_dbi, &dbKey, &dbValue))) {
        LOG_error("database Readding key %s failed, due to %s", name.c_str(), mdb_strerror(rc));
        mdb_txn_abort(txn);
        return ;
    }
    LOG_trace("Database start to decode value");
    fileInfo.decode((char *)dbValue.mv_data, dbValue.mv_size);
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database read Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return ;
    }
}

void DataBase::clearOldKeys() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> keysNeedCleared;
    keysNeedCleared.reserve(m_cache.size() / 2);
    std::time_t time = 0, now = std::time(nullptr);
    for (const auto& pair : m_cache) {
        time = std::stoll(pair.second.substr(0, 10));
        if(now - time >= 12 * 60 * 60 * 1000) {
            keysNeedCleared.push_back(pair.first);
        }
    }
    for (const auto& key : keysNeedCleared) {
        m_cache.erase(key);
    }
}

int DataBase::saveJsonInfo(std::string const& key, std::string const& value) {
    {
        if(m_cache.find(key) != m_cache.end() && m_cache[key].substr(10) == value) {
            return 0;
        }
        //eg. time = 1778943187  len = 10
        std::time_t now = std::time(nullptr);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache[key] = std::to_string(now) + value;
    }
    if(m_cache.size() > 16384) {
        std::thread clear(&DataBase::clearOldKeys, this);
        clear.detach();
    }
    
    int rc = 0;
    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_txn *txn;
    
    LOG_trace("Begin to save json information to database with key = '%s'", key.c_str());

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin write transaction failed, due to %s", mdb_strerror(rc));
        return rc;
    }

    LOG_trace("Begin to save json information write transaction, Key = %s", key.c_str());

    dbKey.mv_size = key.length();
    dbKey.mv_data = (void *)key.c_str();
    dbValue.mv_size = value.length();
    dbValue.mv_data = (void *)value.c_str();

    if((rc = mdb_put(txn, m_dbi, &dbKey, &dbValue, 0))) {
        LOG_error("database Writting json data failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    return rc;
}

std::string DataBase::getJsonInfo(std::string const& key) {
    {
        if(m_cache.find(key) != m_cache.end()) {
            return m_cache[key].substr(10);
        }
    }
    int rc = 0;

    MDB_val dbKey, dbValue;
    MDB_cursor *cursor = NULL;
    MDB_txn *txn;

    LOG_trace("Begin to get json information from database with key = '%s'", key.c_str());

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin read transaction failed, due to %s", mdb_strerror(rc));
        return "";
    }

    LOG_trace("Begin to get json information write transaction with key = '%s'", key.c_str());

    dbKey.mv_size = key.length();
    dbKey.mv_data = (void *)(key.c_str());

    if((rc = mdb_get(txn, m_dbi, &dbKey, &dbValue))) {
        LOG_error("database Readding key %s failed, due to %s", key.c_str(), mdb_strerror(rc));
        mdb_txn_abort(txn);
        return "";
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database read Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return "";
    }
    std::string val((char *)dbValue.mv_data, dbValue.mv_size);
    {
        std::time_t now = std::time(nullptr);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache[key] = std::to_string(now) + val;
    }
    return val;
}



std::string DataBase::getProxyListStr() {
    int rc = 0;

    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_cursor *cursor = NULL;
    MDB_txn *txn;

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin read transaction failed, due to %s", mdb_strerror(rc));
        return "";
    }

    dbKey.mv_size = proxyListKey.length();
    dbKey.mv_data = (void *)(proxyListKey.c_str());

    dbValue.mv_size = 0;
    LOG_trace("database Begin read transaction proxy list");
    if((rc = mdb_get(txn, m_dbi, &dbKey, &dbValue))) {
        LOG_error("database Readding proxy list failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return "";
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database read Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return "";
    }
    return std::string((char *)dbValue.mv_data, dbValue.mv_size);
}

//0000000cL2JhaWR1LwAA2026-04-29 15:31:5400000080{"name":"百度代理配置","requestPath":"/baidu","proxyUrl":"https://www.baidu.com","requestHeaders":{},"responseHeaders":{}}
int DataBase::saveProxy(ProxyInfo & proxyInfo) {
    std::string proxyListStr = getProxyListStr();
    if(!proxyListStr.empty()) {
        ProxyInfo proxy;
        size_t pos = 0, off = 0;
        while(pos < proxyListStr.length()) {
            off = proxy.decode(proxyListStr.c_str() + pos, proxyListStr.length() - pos);
            if(off == 0) {
                LOG_error("parse Proxy list failed, pos = %lu, str = %s", pos, proxyListStr.c_str());
                return -1;
            }
            if(proxy.getName() == proxyInfo.getName()) {
                proxyListStr = proxyListStr.substr(0, pos) + proxyListStr.substr(pos + off);
            }
            pos += off;
        }
    }

    int rc = 0;
    
    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_txn *txn;
    
    LOG_trace("Begin to save proxy information to database");

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin write transaction failed, due to %s", mdb_strerror(rc));
        return rc;
    }

    LOG_trace("Begin to save proxy information write transaction");

    std::string value = proxyListStr + proxyInfo.encode();

    dbKey.mv_size = proxyListKey.length();
    dbKey.mv_data = (void *)proxyListKey.c_str();
    dbValue.mv_size = value.length();
    dbValue.mv_data = (void *)value.c_str();

    if((rc = mdb_put(txn, m_dbi, &dbKey, &dbValue, 0))) {
        LOG_error("database Writting proxy data failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    return rc;
}

std::vector<ProxyInfo> DataBase::listProxy() {
    std::string proxyListStr = getProxyListStr();
    std::vector<ProxyInfo> proxyList;
    size_t pos = 0, off = 0;
    while(pos < proxyListStr.length()) {
        ProxyInfo proxy;
        off = proxy.decode(proxyListStr.c_str() + pos, proxyListStr.length() - pos);
        if(off == 0) {
            LOG_error("parse Proxy list failed, pos = %lu, str = %s", pos, proxyListStr.c_str());
            return std::vector<ProxyInfo>();
        }
        pos += off;
        proxyList.emplace_back(proxy);
    }
    return proxyList;
}

int DataBase::delProxy(std::string const& name) {
    std::string proxyListStr = getProxyListStr();
    bool found = false;
    if(!proxyListStr.empty()) {
        ProxyInfo proxy;
        size_t pos = 0, off = 0;
        while(pos < proxyListStr.length()) {
            off = proxy.decode(proxyListStr.c_str() + pos, proxyListStr.length() - pos);
            if(off == 0) {
                LOG_error("parse Proxy list failed, pos = %lu, str = %s", pos, proxyListStr.c_str());
                return -1;
            }
            if(proxy.getName() == name) {
                found = true;
                break;
            }
            pos += off;
        }
        if(found) {
            proxyListStr = proxyListStr.substr(0, pos) + proxyListStr.substr(pos + off);
        }
    }
    if(!found) {
        return 0;
    }

    int rc = 0;
    
    // MDB_dbi dbi;
    MDB_val dbKey, dbValue;
    MDB_txn *txn;
    
    LOG_trace("Begin to save proxy information to database");

    if((rc = mdb_txn_begin(m_env, NULL, 0, &txn))) {
        LOG_error("database Begin write transaction failed, due to %s", mdb_strerror(rc));
        return rc;
    }

    LOG_trace("Begin to save proxy information write transaction");

    dbKey.mv_size = proxyListKey.length();
    dbKey.mv_data = (void *)proxyListKey.c_str();
    dbValue.mv_size = proxyListStr.length();
    dbValue.mv_data = (void *)proxyListStr.c_str();

    if((rc = mdb_put(txn, m_dbi, &dbKey, &dbValue, 0))) {
        LOG_error("database Writting proxy data failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    if((rc = mdb_txn_commit(txn))) {
        LOG_error("database Commit transaction failed, due to %s", mdb_strerror(rc));
        mdb_txn_abort(txn);
        return rc;
    }
    return rc;
}