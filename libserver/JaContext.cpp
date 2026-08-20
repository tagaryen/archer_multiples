#include "JaContext.h"

using namespace fs::server;


//head_len = 4 + 16 + 32 = 52
void JaContext::sendSuccess() {
    char response[52 + 1];
    memcpy(response, m_header, 52);
    response[52] = SERVER_OK_TYPE;
    base_handler_write(m_channel, response, 53);
}

//head_len = 4 + 16 + 32 = 52
void JaContext::sendSuccess(std::string const& key, std::string const& value) {
    size_t off = 53, len = key.length(), total = 52 + 1 + 2 + key.length() + value.length();
    char *response = (char * )malloc(total);
    memcpy(response, m_header, 52);
    response[52] = SERVER_OK_TYPE;
    response[off++] = (char)((len >> 8) & 0xff);
    response[off++] = (char)(len & 0xff);
    memcpy(response + off, key.c_str(), len);
    off += len;
    memcpy(response + off, value.c_str(), value.length());
    base_handler_write(m_channel, response, total);
    free(response);
}

//head_len = 4 + 16 + 32 = 52
void JaContext::sendError(char type) {
    char response[52 + 1];
    memcpy(response, m_header, 52);
    response[52] = type;
    base_handler_write(m_channel, response, 53);
}

//head_len = 4 + 16 + 32 = 52
//  magic        nonce        sig        msgType     keyLen          key             data
//    4     +     16     +     32     +     1     +     2     +     keyLen     +     data
// 9,6,0,7  
int JaContext::checkHeader(char *data, size_t data_len, const char *key) {
    if(data_len < 4 + 16 + 32 + 1 + 2 + 1) {
        return -1;
    }
    if(memcmp(JA_MAGIC, data, 4) != 0) {
        return -1;
    }
    int off = 4;
    uint8_t nonce[16];
    memcpy(nonce, data + off, 16);
    off += 16;

    uint8_t cipher[32];
    memcpy(cipher, data + off, 32);
    off += 32;

    uint8_t *text = NULL;
    size_t text_len = 0;
    if(!sm4_decrypt((uint8_t *)key, cipher, 32, &text, &text_len)) {
        return -1;
    }
    int ret = 0;
    if(text_len != 16 || memcmp(text, nonce, 16) != 0) {
        ret = -1;
    }
    if(text) {
        free(text);
    }
    m_header = data;
    return ret;
}

