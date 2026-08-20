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
#include <iostream>

static const int HEX_MAP[] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 127, 127, 127, 127, 127, 127, 127, 10, 11, 12, 13, 14, 15, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 10, 11, 12, 13, 14, 15, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
static const char BYTE_MAP[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};


int hexToUint8s(std::string const& hex, uint8_t* bytes) {
    for(int i = 0 ; i < hex.length(); i+=2) {
        uint8_t h1 = hex[i], h2 = hex[i+1];
        if(HEX_MAP[h1] == 127 || HEX_MAP[h2] == 127) {
            return 0;
        }
        bytes[i>>1] = (HEX_MAP[h1] << 4) | HEX_MAP[h2];
    }
    return 1;
}

std::string uint8sToHex(const uint8_t* bytes, size_t bytes_len) {
    std::string hex;
    for(int i = 0 ; i < bytes_len; i++) {
        hex += BYTE_MAP[(bytes[i] >> 4) & 0xf];
        hex += BYTE_MAP[bytes[i] & 0xf];
    }
    return hex;
}

std::string sizeToHex(size_t n) {
    char chars[] = {((char) ((n >> 24) & 0xff)), ((char) ((n >> 16) & 0xff)), ((char) ((n >> 8) & 0xff)), ((char) (n & 0xff))};
    return uint8sToHex((uint8_t *) chars, 4);
}

size_t hexToSize(std::string const& str) {
    uint8_t chars[4];
    if(!hexToUint8s(str, chars)) {
        return 0;
    }
    return (((size_t)chars[0]) << 24) | (((size_t)chars[1]) << 16) | (((size_t)chars[2]) << 8) | ((size_t)chars[3]);
}



int main() {


    std::string a = "nihao,shijiea";

    std::string size = sizeToHex(a.length());

    std::string ae = size +a;
    std::cout<<"len = "<<a.length()<<", "<<ae<<std::endl;

    size_t l = hexToSize(ae.substr(0, 8));
    std::cout<<"l = "<<l<<", "<<ae.substr(8)<<std::endl;


    return 0;

}