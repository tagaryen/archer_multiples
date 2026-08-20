#include "archer_net.h"

#include <string>
#include <sys/file.h>
#include <sstream>  
#include <fstream>  
#include <regex>  


void proxyRequestSender(RequestSender *sender, void *arg) {
    printf("Proxy sender\n");
    http_request_sender_send(sender, "nihaow", 6);
    http_request_sender_end(sender);
}
void proxyResponseReader(Response *res, const void *data, const size_t size, void *arg) {
    char *t = (char *) malloc(size + 1);
    memcpy(t, data, size);
    t[size] = '\0';

    printf("reads %s\n", t);
}

void test() {
    std::string url = "https://10.32.123.24:9666/api?t=1234567";
    
    std::string caPath = "/usr/local/rs/gm_cert/ca.crt";
    std::string crtPath = "/usr/local/rs/gm_cert/cli.crt";
    std::string keyPath = "/usr/local/rs/gm_cert/cli.key";
    std::string enCrtPath = "/usr/local/rs/gm_cert/cli_en.crt";
    std::string enKeyPath = "/usr/local/rs/gm_cert/cli_en.key";
    bool verifyPeer = true;
    SSLOption *m_ssloption = NULL;
    if(caPath.empty()) {
        printf("ProxySsl.caPath can not be found");
    } else {
        std::ifstream ca(caPath);
        if(!ca.is_open()) {
            printf("Can not open file %s", crtPath.c_str());
        } else {
            m_ssloption = ssl_option_new(1, verifyPeer);
            std::stringstream caBuffer; 
            caBuffer << ca.rdbuf();
            std::string caStr =  caBuffer.str();
            if(ssl_option_set_trust_ca(m_ssloption, caStr.c_str(), caStr.length()) == 0) {
                printf("Can not set SSL ca file %s", caStr.c_str());
            }
        }
    }

    if(crtPath.empty()) {
        printf("ProxySsl.crtPath can not be found");
    } else if(keyPath.empty()) {
        printf("ProxySsl.keyPath can not be found");
    } else if(m_ssloption) {
        bool ok = true;
        std::ifstream crt(crtPath), key(keyPath);
        if(!crt.is_open()) {
            printf("Can not open file %s", crtPath.c_str());
            ok = false;
        }
        if(!key.is_open()) {
            printf("Can not open file %s", keyPath.c_str());
            ok = false;
        }
        if(ok) {
            std::stringstream crtBuffer, keyBuffer; 
            crtBuffer << crt.rdbuf();
            keyBuffer << key.rdbuf();
            std::string crtStr =  crtBuffer.str(), keyStr = keyBuffer.str();
            if(ssl_option_set_certificate_and_key(m_ssloption, crtStr.c_str(), crtStr.length(), keyStr.c_str(), keyStr.length()) == 0) {
                printf("Can not set SSL Crt and Key file");
            }
        }
        crt.close();
        key.close();
    }

    if(enCrtPath.empty()) {
        printf("ProxySsl.enCrtPath can not be found");
    } else if(enKeyPath.empty()) {
        printf("ProxySsl.enKeyPath can not be found");
    } else if(m_ssloption) {
        bool ok = true;
        std::ifstream enCrt(enCrtPath), enKey(enKeyPath);
        if(!enCrt.is_open()) {
            printf("Can not open file %s", enCrtPath.c_str());
            ok = false;
        }
        if(!enKey.is_open()) {
            printf("Can not open file %s", enKeyPath.c_str());
            ok = false;
        }
        if(ok) {
            std::stringstream crtBuffer, keyBuffer; 
            crtBuffer << enCrt.rdbuf();
            keyBuffer << enKey.rdbuf();
            std::string crtStr =  crtBuffer.str(), keyStr = keyBuffer.str();
            if(ssl_option_set_encrypt_certificate_and_key(m_ssloption, crtStr.c_str(), crtStr.length(), keyStr.c_str(), keyStr.length()) == 0) {
                printf("Can not set Encrypt SSL Crt and Key file");
            }
        }
        enCrt.close();
        enKey.close();
    }
    
    HttpClientOption *opt = http_client_opt_new_method("POST");

    http_client_opt_add_header(opt, "connection", "keep-alive");
    http_client_opt_add_header(opt, "content-type", "application/x-www-form-urlencoded");
    http_client_opt_add_header(opt, "accept", "text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2");
    http_client_opt_add_header(opt, "content-length", "6");
    http_client_opt_add_header(opt, "content-encoding", "utf-8");

    printf("Proxy request url = %s \n", url.c_str());
    if(m_ssloption) {
        http_client_opt_set_ssloption(opt, m_ssloption);
    }

    http_client_stream_request(url.c_str(), opt, proxyRequestSender, proxyResponseReader);
    http_client_opt_free(opt);
}

int main() {
    test();
    return 0;
}