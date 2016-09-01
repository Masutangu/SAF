//
// Created by Masutangu on 16/8/23.
//

#ifndef SAF_CLIENTCONTEXT_H
#define SAF_CLIENTCONTEXT_H

extern "C" {
#include <uv.h>
}

#include "msg.h"
#include "buffer.h"

namespace saf {

class Action;
class Handler;
class AsyncServer;

class ClientContext {
public:
    friend class AsyncServer;
    ClientContext():m_handler(NULL), m_msg(NULL), m_state_idx(0), m_action_idx(0) {}

    int processRequest(char* buf, unsigned int len);
    void sendResponse(char* buf, unsigned int len);

    void processAction(Action* action, char* buf, unsigned int len);
    void finishAction();

    ~ClientContext();

public:
    Buffer m_read_buf;
    Buffer m_write_buf;
    Handler* m_handler;
    int m_state_idx;
    int m_action_idx;
    Msg* m_msg;
    AsyncServer* m_server_ctx;
    uv_write_t m_write_req;
    uv_stream_t m_client_handle;

};

}


#endif //SAF_CLIENTCONTEXT_H
