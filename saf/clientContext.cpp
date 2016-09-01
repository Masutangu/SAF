//
// Created by Masutangu on 16/8/23.
//

#include "clientContext.h"
#include "action.h"
#include "server.h"
#include "utils.h"
#include "common.h"

#include <cstdlib>

namespace saf {

struct ActionContext {
    Action* action;
    Buffer send_buf;
    Buffer recv_buf;
    uv_connect_t connect_req;
    uv_timer_t time_handle;
    uv_tcp_t action_handle;
    uv_write_t write_req;
    ClientContext* c_ctx;
};

static void sendRespFinish(uv_write_t *req, int status) {
    printf("finish write status=%d\n", status);
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
}

void ClientContext::sendResponse(char *buf, unsigned int len) {
    uv_buf_t wrbuf = uv_buf_init(buf, len);
    uv_write(&m_write_req, &m_client_handle, &wrbuf, 1, sendRespFinish);
}

int ClientContext::processRequest(char *buf, unsigned int len) {
    int ret = m_handler->prepareProcess(m_read_buf.data(), m_read_buf.len(), m_msg);
    if (ret == 0) {
        // 收包完整,清除读buffer
        m_read_buf.clear();
        m_handler->process(this);
        return 0;
    } else if (ret < 0) {
        // 出错,断开客户端连接
        m_handler->handleError(UNPACK_HANDLER_REQ_ERR, ret);
        return -1;
    } else {
        return 1;
    }
}

static void freeActionContext(uv_handle_t* server) {
    printf("freeActionContext\n");
    if (server->data) {
        ActionContext* a_ctx = (ActionContext*) server->data;
        uv_timer_stop(&(a_ctx->time_handle));
        a_ctx->action->finish(a_ctx->c_ctx);
        server->data = NULL;
        delete a_ctx;
    }
}

/*
 * You can free the handle in the request callback, and the connect request
 * in the connect callback. If you close the handle the connect callback
 * will be called with UV_ECANCELED (IIRC), so it;s guaranteed that you'll
 * be able to free it there.
 */
static void closeActionConn(uv_stream_t* server) {
    printf("closeActionConn\n");
    if (server) {
        uv_close((uv_handle_t*) server, freeActionContext);
    }
}

static void actionTimeout(uv_timer_t* handle) {
    printf("actionTimeout\n");
    ActionContext* a_ctx = (ActionContext*) handle->data;
    closeActionConn((uv_stream_t*) &a_ctx->action_handle);
}

static void recvActionRsp(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf) {
    printf("recvActionRsp\n");
    ActionContext* a_ctx = (ActionContext*) server->data;
    if (nread < 0) {
        if (nread != UV_EOF) {
            fprintf(stderr, "error onRecv nread=%ld EOF=%d\n", nread, UV_EOF);
            a_ctx->action->handleError(RECV_ACTION_REQ_ERR, nread);
        }
        closeActionConn(server);
        return;
    }
    a_ctx->recv_buf.append(buf->base, nread);
    if (buf->base)
        free(buf->base);

    ClientContext* c_ctx = a_ctx->c_ctx;
    int ret = a_ctx->action->afterProcess(a_ctx->recv_buf.data(), a_ctx->recv_buf.len(), c_ctx->m_msg);
    if (ret > 0) {
        // 收包未完整, 继续收
        return;
    } else if (ret < 0) {
        // 报错
        a_ctx->action->handleError(UNPACK_ACTION_RSP_ERR, ret);
    }
    // 收包成功或者解析失败,调用 closeActionConn
    closeActionConn(server);
}

static void sendActionReqFinish(uv_write_t *req, int status) {
    printf("sendActionReqFinish\n");
    if (status < 0) {
        ActionContext* a_ctx = (ActionContext*) req->handle->data;
        a_ctx->action->handleError(SEND_ACION_REQ_ERR, status);
        closeActionConn(req->handle);
        return;
    }
    uv_read_start((uv_stream_t*)req->handle, allocBuffer, recvActionRsp);
}

static void establishActionConn(uv_connect_t* req, int status) {
    printf("establishActionConn\n");
    ActionContext* a_ctx = (ActionContext*) req->data;
    if (status < 0) {
        printf("New connection error %s\n", uv_strerror(status));
        a_ctx->action->handleError(CONN_ACTION_ERR, status);
        freeActionContext((uv_handle_t*) req->handle);
        return;
    }

    uv_buf_t buf = uv_buf_init(a_ctx->send_buf.data(), a_ctx->send_buf.len());
    uv_write(&a_ctx->write_req, req->handle, &buf, 1, sendActionReqFinish);
}

void ClientContext::processAction(Action* action, char* buf, unsigned int len) {
    ActionContext *a_ctx = new ActionContext;
    a_ctx->c_ctx = this;
    uv_tcp_init(m_server_ctx->m_main_loop, &a_ctx->action_handle);
    a_ctx->action_handle.data = a_ctx;

    struct sockaddr_in dest;
    uv_ip4_addr(action->getIp(), action->getPort(), &dest);

    a_ctx->action = action;
    a_ctx->send_buf.append(buf, len);

    if (action->m_timeout > 0) {
        a_ctx->time_handle.data = a_ctx;
        uv_timer_init(m_server_ctx->m_main_loop, &a_ctx->time_handle);
        uv_timer_start(&a_ctx->time_handle, actionTimeout, action->m_timeout, 0);
    }

    a_ctx->connect_req.data = a_ctx;
    uv_tcp_connect(&a_ctx->connect_req, &a_ctx->action_handle, (struct sockaddr *) &dest, establishActionConn);
}

ClientContext::~ClientContext() {
    if (m_msg) {
        delete m_msg;
    }
}


}