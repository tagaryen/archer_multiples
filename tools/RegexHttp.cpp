#include <iostream>
#include <regex>
#include <string>

bool isUrl(const std::string& url) {
    if(url.length() < 10) {
        return false;
    }
    bool ssl = false;
    std::string host = "";
    if(url.substr(0, 8) == "https://") {
        ssl = true;
        host = url.substr(8);
    } else if(url.substr(0, 7) == "http://") {
        host = url.substr(7);
    } else {
        return false;
    }
    size_t idx = host.find_first_of('/');
    if(idx != std::string::npos) {
        host = host.substr(0, idx);
    }
    idx = host.find_first_of(':');
    if(idx == std::string::npos) {
        host += ssl?":443":":80";
    }
    //m_host = host;

    printf("host = %s \n", host.c_str());
    return true;
}

bool isHttpOrHttpsUrl(const std::string& url) {
    //                               ([a-zA-Z0-9-]+\.)*[a-zA-Z0-9-]+
    // std::regex pattern(R"(^https?://(([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}|([0-9]{1,3}\.){3}[0-9]{1,3})(:[0-9]{1,5})?(/.*)?$)");
    std::regex pattern(R"(^https?://(([a-zA-Z0-9-]+\.)*[a-zA-Z0-9-]+|([0-9]{1,3}\.){3}[0-9]{1,3})(?::[0-9]{1,5})?(/.*)?$)");
    std::smatch match;
    std::string host = "";
    if (std::regex_match(url, match, pattern)) {
        host = match[1].str();
        if(match[2].matched) {
            host += ":";
            host += match[2].str();
        }
        printf("host = %s \n", host.c_str());
        return true;
    } else {
        printf("Proxy can not parse URL %s and get host \n", url.c_str());
        return false;
    }
}

bool isHttpUrl(const std::string& url) {
    std::regex re(R"(^(?:https?://)?((?:[^/:]+|\[[^\]]+\]))(?::(\d+))?)");
    std::smatch match;
    std::string host = "";
    if (std::regex_match(url, match, re)) {
        host = match[1].str();
        if(match[3].matched) {
            host += ":";
            host += match[3].str();
        }
        printf("host = %s \n", host.c_str());
        return true;
    } else {
        printf("Proxy can not parse URL %s and get host \n", url.c_str());
        return false;
    }
}

int main() {
    // 测试用例
    std::vector<std::string> testUrls = {
        "https://10.32.123.24:9888/api",
        "http://example.com",                // ✅ 域名，无端口
        "https://example.com:8080",          // ✅ 域名，带端口
        "http://192.168.1.1",                // ✅ IPv4，无端口
        "https://192.168.1.1:443",           // ✅ IPv4，带端口
        "http://sub.domain.co.uk/path?q=1",  // ✅ 域名，带路径和查询参数
        "ftp://example.com",                 // ❌ 协议不对
        "http://",                           // ❌ 缺少主机
        "http://.com",                       // ❌ 域名不合法
        "http://256.1.1.1",                  // ❌ IP 超出范围（正则只检查格式，不验证数值）
        "https://localhost",                 // ✅ localhost 不包含点，按本正则视为非法，若需要可放宽规则
        "http://example.com:99999",          // ❌ 端口超出常规范围（正则限制1-5位数字，但数值范围未做语义检查）
    };

    for (const auto& url : testUrls) {
        std::cout << (isUrl(url) ? "[OK] " : "[NO] ") << url << std::endl;
    }

    return 0;
}

/**
[OK] http://example.com
[OK] https://example.com:8080
[OK] http://192.168.1.1
[OK] https://192.168.1.1:443
[OK] http://sub.domain.co.uk/path?q=1
[NO] ftp://example.com
[NO] http://
[NO] http://.com
[OK] http://256.1.1.1
[NO] https://localhost
[OK] http://example.com:99999 


[OK] http://example.com
[OK] https://example.com:8080
[OK] http://192.168.1.1
[OK] https://192.168.1.1:443
[OK] http://sub.domain.co.uk/path?q=1
[NO] ftp://example.com
[NO] http://
[NO] http://.com
[OK] http://256.1.1.1
[OK] https://localhost
[OK] http://example.com:99999
*/