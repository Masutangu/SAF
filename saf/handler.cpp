//
// Created by Masutangu on 16/8/17.
//

#include "handler.h"
#include "server.h"
#include "common.h"

#include <cstdlib>
#include <cstddef>

namespace saf {

void Handler::process(ClientContext* c_ctx) {
    printf("handler: process state list size=%ld curr idx=%d\n", m_state_list.size(), c_ctx->m_state_idx);
    if (c_ctx->m_state_idx < m_state_list.size()) {
        State* state = m_state_list[c_ctx->m_state_idx++];
        // 执行 state 前, 将 action_idx 置 0
        c_ctx->m_action_idx = 0;
        int ret = state->prepareProcess(c_ctx->m_msg);
        if (ret == 0) {
            state->process(c_ctx);
        } else {
            state->handleError(UNPACK_STATE_REQ_ERR, ret);
            // 执行下一个state
            process(c_ctx);
        }
    } else {
        static char buf[DEFAULT_BUF_SIZE];
        char* actual_buf = buf;
        unsigned int actual_len = DEFAULT_BUF_SIZE;

        int ret = afterProcess(actual_buf, actual_len, c_ctx->m_msg);
        if (ret > 0) {
            actual_buf = new char[actual_len];
            ret = afterProcess(actual_buf, actual_len, c_ctx->m_msg);
        }
        if (ret < 0) {
            handleError(PACK_HANDLER_RSP_ERR, ret);
            actual_len = 0;
        }
        c_ctx->sendResponse(actual_buf, actual_len);

        if (actual_len > DEFAULT_BUF_SIZE) {
            delete []actual_buf;
        }
    }
}

Handler::~Handler() {
    for (int i = 0; i < m_state_list.size(); i++) {
        if (m_state_list[i]) {
            delete m_state_list[i];
        }
    }
}


}
