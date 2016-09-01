//
// Created by Masutangu on 16/8/26.
//

#ifndef SAF_COMMON_H
#define SAF_COMMON_H

namespace saf {

const unsigned int DEFAULT_BUF_SIZE = 1024;

enum ACTION_ERROR_ENUM {
    PACK_ACTION_REQ_ERR = 1,   // 打包 Action 请求包出错
    UNPACK_ACTION_RSP_ERR,     // 解包 Action 回包出错
    ACTION_TIMEOUT_ERR,        // action 超时
    CONN_ACTION_ERR,           // action 连接出错
    SEND_ACION_REQ_ERR,        // action 发送请求包出错
    RECV_ACTION_REQ_ERR,       // action 接收请求包出错
};

enum STATE_ERROR_ENUM {
    UNPACK_STATE_REQ_ERR = 1,
    PACK_STATE_RSP_ERR = 2,
};

enum HANDLER_ERROR_ENUM {
    UNPACK_HANDLER_REQ_ERR = 1,
    PACK_HANDLER_RSP_ERR = 2,
};

}

#endif //SAF_COMMON_H
