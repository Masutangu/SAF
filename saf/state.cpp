//
// Created by Masutangu on 16/8/15.
//

#include "state.h"
#include "common.h"
#include "server.h"

namespace saf {

void State::addAction(Action* action) {
    m_action_list.push_back(action);
    action->m_state = this;
}

void State::process(ClientContext* c_ctx) {
    // 如果没有action,直接finish
    if (m_action_list.size() == 0) {
        finish(c_ctx);
        return;
    }

    static char buf[DEFAULT_BUF_SIZE];
    char* actual_buf = NULL;
    unsigned int actual_len = 0; // buf 的实际长度
    int ret = 0;

    for(unsigned int i = 0; i < m_action_list.size(); i++) {
        Action* action = m_action_list[i];
        actual_len = DEFAULT_BUF_SIZE;
        actual_buf = buf;
        ret = action->prepareProcess(actual_buf, actual_len, c_ctx->m_msg);

        if (ret > 0) {
            // ret > 0, buf 长度不够
            actual_buf = new char[actual_len];
            ret = action->prepareProcess(actual_buf, actual_len, c_ctx->m_msg);
        }
        if (ret < 0) {
            // ret < 0 打包出错, 回调 handleError, 并调用 action->finish() 结束该 action
            action->handleError(PACK_ACTION_REQ_ERR, ret);
            action->finish(c_ctx);
        }
        if (ret == 0) {
            c_ctx->processAction(action, actual_buf, actual_len);
        }
        if (actual_len > DEFAULT_BUF_SIZE) {
            delete []actual_buf;
        }
    }
}

void State::finishAction(ClientContext* c_ctx) {
    printf("finishAction\n");
    c_ctx->m_action_idx++;
    if (c_ctx->m_action_idx >= m_action_list.size()) {
        finish(c_ctx);
    }
}

void State::finish(ClientContext* c_ctx) {
    int ret = afterProcess(c_ctx->m_msg);
    if (ret != 0) {
        handleError(PACK_STATE_RSP_ERR, ret);
    }
    m_handler->process(c_ctx);

}

State::~State() {
    for (int i = 0; i < m_action_list.size(); i++) {
        if (m_action_list[i]) {
            delete m_action_list[i];
        }
    }
}

}
