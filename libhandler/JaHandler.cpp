#include "JaHandler.h"
#include <sys/file.h>

using namespace fs::handler;


void JaHandler::handleMessage(server::JaContext &ctx, char *data, size_t dataLen) {
    uint8_t type = data[0];
    size_t keyLen = (((uint32_t)data[1]) << 8) |  ((uint32_t)data[2]);
    if(dataLen < keyLen + 3) {
        LOG_warn("Receive invalid data length = '%d'", dataLen);
        ctx.sendError((char)ERROR_BODY_TYPE);
        return ;
    }
    std::string key(data + 3, keyLen);
    switch(type) {
        case CLIENT_GET_TYPE: {
            std::string value = m_database.getJsonInfo(key);
            ctx.sendSuccess(key, value);
            break;
        }
        case CLIENT_SAVE_TYPE: {
            std::string value(data + (3 + keyLen), dataLen - 3 - keyLen);
            if(m_database.saveJsonInfo(key, value)) {
                LOG_warn("RPC JaHandler save JSON fail, key:'%s'", key.c_str());
                ctx.sendError((char)SERVER_FAIL_TYPE);
            } else {
                LOG_info("RPC JaHandler save JSON success, key:'%s'", key.c_str());
                ctx.sendSuccess();
            }
            break;
        }
        default: {
            LOG_warn("Receive invalid message type = '%d'", type);
            ctx.sendError((char)ERROR_TYPE_TYPE);
            break;
        }
    }
}
