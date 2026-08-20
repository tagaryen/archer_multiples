#include "archer_alg.h"
#include <string>
#include <iostream>


int main() {
    std::string nonce = "1234534890123456";
    std::string key = "xuyishigeshuaige";
    uint8_t *out = NULL;
    size_t out_size = 0;
    sm4_encrypt((uint8_t*) key.c_str(), (uint8_t *)nonce.c_str(), nonce.length(), &out, &out_size);
    std::cout<<"size  = "<<out_size<<std::endl;

    
    uint8_t *text = NULL;
    size_t text_size = 0;
    if(!sm4_decrypt((uint8_t*) key.c_str(), out, out_size, &text, &text_size)) {
        std::cout<<"decrypt failed"<<std::endl;
    }
    if(text) {
        std::cout<<std::string((char *)text, text_size)<<std::endl;
    }
    return 1;
}