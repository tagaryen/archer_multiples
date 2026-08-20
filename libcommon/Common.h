#pragma once

#include <functional>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <regex>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <sys/stat.h>
#include <direct.h> 
#define MKDIR(path) _mkdir(path.c_str())
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path.c_str(), 0755)
#endif

#include "archer_alg.h"

#pragma warning(push)
#pragma GCC diagnostic push
#pragma warning(pop)
#pragma GCC diagnostic pop                  

namespace fs 
{
namespace common 
{
bool isAbsolutePath(std::string const& path);
void createDirectories(std::string const& path);
std::string getCurrentPath();
bool fileExists(const std::string& filename);
std::string getNowTime();
long long timeToLong(std::string const& s);
int hexToUint8s(std::string const& hex, uint8_t* bytes);
std::string uint8sToHex(const uint8_t* bytes, size_t bytes_len);
EcPublicKey * getPublicKeyFromHex(std::string const& hex);
EcSignature * getSignatureFromHex(std::string const& hex);
std::string base64Encode(const std::vector<uint8_t>& data);
std::string base64Encode(const std::string& data); 
std::vector<uint8_t> base64Decode(const std::string& encoded_string);
std::string base64DecodeToString(const std::string& encoded_string);
std::string sizeToHex(size_t n);
size_t hexToSize(std::string const& str);
bool checkFullUrl(const std::string& url);
bool isDirectory(const std::string& path);
bool createDirectories2(const std::string& path);
std::string urldecode(const std::string& str);
std::string urlencode(const std::string& str);
}
}

