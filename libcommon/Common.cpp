#include "Common.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>

#define FROM_HEX(x) (('A' <= x && x <= 'Z')?(x - 'A' + 10):(('a' <= x && x <= 'z')?(x - 'a' + 10):(('0' <= x && x <= '9')?(x -'0'):x)))

static const int HEX_MAP[] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 127, 127, 127, 127, 127, 127, 127, 10, 11, 12, 13, 14, 15, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 10, 11, 12, 13, 14, 15, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
static const char BYTE_MAP[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};


static int checkHex(std::string const& hex) {
    if(hex.length() != 128 && hex.length() != 130) {
        return -1;
    }
    if(hex.length() == 130){
        if(hex[0] != '0' || (hex[1] != 'x' && hex[1] != 'X')) {
            return -1;
        }
        return 2;
    }
    return 0;
}


static void createDirectory(const char *path) {
    if(access(path, 0)) {
#ifdef _WIN32
        mkdir(path);
#else
        mkdir(path, S_IRWXU);
#endif
    }
}

void fs::common::createDirectories(std::string const& pathStr) {
    const char *path = pathStr.c_str();
    size_t len = pathStr.length();
    char *dir = (char *) malloc(len + 1);
    memcpy(dir, path, len);
    dir[len] = '\0';
    if(dir[len - 1] == 92 || dir[len - 1] == 47) {
        dir[len - 1] = '\0';
        --len;
    }
    for(int i = 1; i < len; i++) {
        if(dir[i] == 47 || dir[i] == 92) {
            dir[i] = 0;
            createDirectory(dir);
            dir[i] = '/';
        }
    }
    createDirectory(dir);
}



bool fs::common::isAbsolutePath(std::string const& path) {
    if(path.empty()) {
        return false;
    }
#ifdef __WIN32
    return (isalpha(path[0]) && path[1] == ':') || (path[0] == 92 && path[1] == 92);
#else
    return (path[0] == '/');
#endif
}

std::string fs::common::getCurrentPath() {
    char path[1024];
    if(getcwd(path, 1024)) {}
    size_t len = strlen(path);
    return std::string(path, len);
}

bool fs::common::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    bool exists = file.good();
    file.close();
    return exists;
}

std::string fs::common::getNowTime() {
    std::time_t now = std::time(nullptr);
    // now += 8 * 3600;//时区问题
    std::tm *localTime = std::localtime(&now);
    char timestr[20];
    std::strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", localTime);
    std::string nowStr(timestr, 19);
    return std::move(nowStr);
}

long long fs::common::timeToLong(std::string const& str) {
    struct tm tm_;
    int year, month, day, hour, minute, second;
    sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;
    time_t timeStamp = mktime(&tm_);
    return timeStamp;
}

int fs::common::hexToUint8s(std::string const& hex, uint8_t* bytes) {
    for(int i = 0 ; i < hex.length(); i+=2) {
        uint8_t h1 = hex[i], h2 = hex[i+1];
        if(HEX_MAP[h1] == 127 || HEX_MAP[h2] == 127) {
            return 0;
        }
        bytes[i>>1] = (HEX_MAP[h1] << 4) | HEX_MAP[h2];
    }
    return 1;
}

std::string fs::common::uint8sToHex(const uint8_t* bytes, size_t bytes_len) {
    std::string hex;
    for(int i = 0 ; i < bytes_len; i++) {
        hex += BYTE_MAP[(bytes[i] >> 4) & 0xf];
        hex += BYTE_MAP[bytes[i] & 0xf];
    }
    return hex;
}

EcPublicKey * fs::common::getPublicKeyFromHex(std::string const& hex) {
    int rt = checkHex(hex);
    if(rt < 0) {
        return NULL;
    }
    std::string hexDst = hex;
    if(rt > 0) {
        hexDst = hex.substr(rt);
    }
    EcPublicKey *key = (EcPublicKey *)std::malloc(sizeof(EcPublicKey));

    for(int i = 0; i < 64; i+=2) {
        unsigned char h1 = hexDst[i], h2 = hexDst[i+1];
        if(HEX_MAP[h1] == 127 || HEX_MAP[h2] == 127) {
            return NULL;
        }
        key->x[i << 1] = (HEX_MAP[h1] << 4) | HEX_MAP[h2];
    }
    for(int i = 64; i < 128; i+=2) {
        unsigned char h1 = hexDst[i], h2 = hexDst[i+1];
        if(HEX_MAP[h1] == 127 || HEX_MAP[h2] == 127) {
            return NULL;
        }
        key->y[i << 1] = (HEX_MAP[h1] << 4) | HEX_MAP[h2];
    }
    return key;
}

EcSignature * fs::common::getSignatureFromHex(std::string const& hex) {
    int rt = checkHex(hex);
    if(rt < 0) {
        return NULL;
    }
    std::string hexDst = hex;
    if(rt > 0) {
        hexDst = hex.substr(rt);
    }
    EcSignature *sig = (EcSignature *)std::malloc(sizeof(EcSignature));
    for(int i = 0; i < 64; i+=2) {
        unsigned char h1 = hexDst[i], h2 = hexDst[i+1];
        if(HEX_MAP[h1] == 127 || HEX_MAP[h2] == 127) {
            return NULL;
        }
        sig->r[i << 1] = (HEX_MAP[h1] << 4) | HEX_MAP[h2];
    }
    for(int i = 64; i < 128; i+=2) {
        unsigned char h1 = hexDst[i], h2 = hexDst[i+1];
        if(HEX_MAP[h1] == 127 || HEX_MAP[h2] == 127) {
            return NULL;
        }
        sig->s[i << 1] = (HEX_MAP[h1] << 4) | HEX_MAP[h2];
    }
    return sig;
}


static const std::string base64_chars =  
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"  
    "abcdefghijklmnopqrstuvwxyz"  
    "0123456789+/";  
  
static inline bool is_base64(unsigned char c) {  
    return (isalnum(c) || (c == '+') || (c == '/'));  
}  
  
std::string fs::common::base64Encode(const std::vector<uint8_t>& data) {  
    std::string ret;  
    size_t i = 0;  
    uint8_t char_array_3[3];  
    uint8_t char_array_4[4];  
  
    while (data.size() > i) {  
        char_array_3[0] = i < data.size() ? data[i++] : 0;  
        char_array_3[1] = i < data.size() ? data[i++] : 0;  
        char_array_3[2] = i < data.size() ? data[i++] : 0;  
  
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
        char_array_4[3] = char_array_3[2] & 0x3f;  
  
        for (size_t j = 0; j < 4; ++j)  
            ret += base64_chars[char_array_4[j]];  
  
        // 处理 padding  
        if (i > data.size()) {  
            if (i - data.size() == 1) {  
                ret[ret.size() - 1] = '=';  
                ret[ret.size() - 2] = '=';  
            } else if (i - data.size() == 2) {  
                ret[ret.size() - 1] = '=';  
            }  
        }  
    }  
  
    return ret;  
}  
  
std::string fs::common::base64Encode(const std::string& data) {  
    std::vector<uint8_t> vec(data.begin(), data.end());  
    return base64Encode(vec);  
}  
  
std::vector<uint8_t> fs::common::base64Decode(const std::string& encoded_string) {  
    size_t in_len = encoded_string.size();  
    size_t i = 0;  
    size_t j = 0;  
    size_t in_ = 0;  
    uint8_t char_array_4[4], char_array_3[3];  
    std::vector<uint8_t> ret;  
  
    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {  
        char_array_4[i++] = encoded_string[in_]; in_++;  
        if (i == 4) {  
            for (i = 0; i < 4; ++i)  
                char_array_4[i] = static_cast<uint8_t>(base64_chars.find(char_array_4[i]));  
  
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);  
            char_array_3[1] = ((char_array_4[1] & 0x0f) << 4) + ((char_array_4[2] & 0x3c) >> 2);  
            char_array_3[2] = ((char_array_4[2] & 0x03) << 6) + char_array_4[3];  
  
            for (i = 0; (i < 3); ++i)  
                ret.push_back(char_array_3[i]);  
            i = 0;  
        }  
    }  
  
    if (i) {  
        for (j = i; j < 4; ++j)  
            char_array_4[j] = 0;  
  
        for (j = 0; j < 4; ++j)  
            char_array_4[j] = static_cast<uint8_t>(base64_chars.find(char_array_4[j]));  
  
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);  
        char_array_3[1] = ((char_array_4[1] & 0x0f) << 4) + ((char_array_4[2] & 0x3c) >> 2);  
        char_array_3[2] = ((char_array_4[2] & 0x03) << 6) + char_array_4[3];  
  
        for (j = 0; (j < i - 1); ++j)  
            ret.push_back(char_array_3[j]);  
    }  
  
    return ret;  
}  

std::string fs::common::base64DecodeToString(const std::string& encoded_string) {  
    auto decoded = base64Decode(encoded_string);  
    return std::string(decoded.begin(), decoded.end());  
}  

std::string fs::common::sizeToHex(size_t n) {
    char chars[] = {((char) ((n >> 24) & 0xff)), ((char) ((n >> 16) & 0xff)), ((char) ((n >> 8) & 0xff)), ((char) (n & 0xff))};
    return uint8sToHex((uint8_t *) chars, 4);
}

size_t fs::common::hexToSize(std::string const& str) {
    uint8_t chars[4];
    if(!hexToUint8s(str, chars)) {
        return 0;
    }
    return (((size_t)chars[0]) << 24) | (((size_t)chars[1]) << 16) | (((size_t)chars[2]) << 8) | ((size_t)chars[3]);
}

bool fs::common::checkFullUrl(const std::string& url) {
    // std::regex pattern(R"(^https?://(([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}|([0-9]{1,3}\.){3}[0-9]{1,3})(:[0-9]{1,5})?(/.*)?$)");
    std::regex pattern(R"(^https?://(([a-zA-Z0-9-]+\.)*[a-zA-Z0-9-]+|([0-9]{1,3}\.){3}[0-9]{1,3})(:[0-9]{1,5})?(/.*)?$)");
    return std::regex_match(url, pattern);
}


bool fs::common::isDirectory(const std::string& path) {
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
#endif
}

bool fs::common::createDirectories2(const std::string& path) {
    std::string stdPath = path;
    std::replace(stdPath.begin(), stdPath.end(), '\\', '/');

    if (isDirectory(stdPath)) 
        return true;

    size_t pos = 0;
    std::string buildPath;
    while ((pos = path.find_first_of("/", pos + 1)) != std::string::npos) {
        buildPath = path.substr(0, pos);
        if (!buildPath.empty() && !isDirectory(buildPath)) {
            if (MKDIR(path) == 0) {
                continue;
            } else {
                return false;
            }
        }
    }
}

std::string fs::common::urldecode(const std::string& str) {
    std::string result;
    result.reserve(str.size()+ 32);
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '%' && i + 2 < str.size()) {
            char c1 = str[i+1];
            char c2 = str[i+2];
            if (std::isxdigit(static_cast<unsigned char>(c1)) &&
                std::isxdigit(static_cast<unsigned char>(c2))) {
                unsigned char value = (FROM_HEX(c1) << 4) | FROM_HEX(c2);
                result.push_back(static_cast<char>(value));
                i += 2;
                continue;
            }
        }
        else if (c == '+') {
            result.push_back(' ');
        }
        else {
            result.push_back(c);
        }
    }
    return result;
}

std::string fs::common::urlencode(const std::string& str) {
    static const char hexDigits[] = "0123456789ABCDEF";
    std::string result;
    result.reserve(str.size() * 3);
    for (unsigned char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result.push_back(static_cast<char>(c));
        } else {
            result.push_back('%');
            result.push_back(hexDigits[c >> 4]);
            result.push_back(hexDigits[c & 0x0F]);
        }
    }
    return result;
}