//
// Created by Masutangu on 16/8/9.
//

#include "server.h"
#include "handlerFactory.h"
#include "headerParser.h"
#include "utils.h"
#include "common.h"

#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <cassert>

namespace saf {

static void freeClientContext(uv_handle_t* client) {
    if (client->data) {
        delete (ClientContext*) client->data;;
    }
}

static void closeClientConn(uv_stream_t* client) {
    uv_close((uv_handle_t*) client, freeClientContext);
}

static void processReq(uv_stream_t *client, ssize_t nread, const uv_buf_t *read_buf) {
    printf("processReq, nread=%ld\n", nread);
    if (nread < 0) {
        if (nread != UV_EOF)
            printf("Read error %s\n", uv_err_name(nread));
        closeClientConn(client);
    } else if (nread > 0) {
        ClientContext* c_ctx = (ClientContext* )client->data;
        c_ctx->m_read_buf.append(read_buf->base, nread);
        int ret = 0;
        do {
            if (c_ctx->m_handler == NULL) {
                // 还未解析出相应的请求包头,调用processHeader
                ret = HEADER_PARSER_INSTANCE.parseHeader(c_ctx->m_read_buf.data(), c_ctx->m_read_buf.len());
                if (ret == 0) {
                    // 收包未完整 继续收包
                    break;
                } else if (ret > 0) {
                    Handler* handler = HANDLER_FACTORY_INSTANCE.genHandlerByReq(ret);
                    if (handler == NULL) {
                        printf("CMD %d can't find response handler\n", ret);
                        closeClientConn(client);
                        break;
                    }
                    c_ctx->m_msg = handler->createMsg();
                    c_ctx->m_handler = handler;

                } else {
                    printf("CMD error %d\n", ret);
                    closeClientConn(client);
                    break;
                }
            }
            //已经有相应的handler
            ret = c_ctx->processRequest(c_ctx->m_read_buf.data(), c_ctx->m_read_buf.len());
            if (ret < 0) {
                // 出错,断开客户端连接
                printf("processRequest error\n");
                closeClientConn(client);
                break;
            }
        } while(0);
    }
    if (read_buf->base) {
        delete []read_buf->base;
    }
}

static void onNewConnection(uv_stream_t *server, int status) {
    printf("NewConnection\n");
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }
    AsyncServer* server_ctx = (AsyncServer*) server->data;
    ClientContext* c_ctx = new ClientContext;
    c_ctx->m_server_ctx = server_ctx;
    c_ctx->m_client_handle.data = c_ctx;
    uv_tcp_init(server_ctx->m_main_loop, (uv_tcp_t*) &c_ctx->m_client_handle);

    if (uv_accept(server, &c_ctx->m_client_handle) == 0) {
        uv_read_start(&c_ctx->m_client_handle, allocBuffer, processReq);
    }
    else {
        printf("accept error call closeClientConn\n");
        closeClientConn((uv_stream_t*) &c_ctx->m_client_handle);
    }
}

int AsyncServer::run() {
    m_main_loop = uv_default_loop();
    assert(m_main_loop);
    uv_tcp_init(m_main_loop, &m_server_handle);
    m_server_handle.data = this;

    struct sockaddr_in addr;
    uv_ip4_addr(m_ip.c_str(), m_port, &addr);
    uv_tcp_bind(&m_server_handle, (const struct sockaddr*)&addr, 0);

    int ret = uv_listen((uv_stream_t*) &m_server_handle, 128, onNewConnection);

    if (ret) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(ret));
        return 1;
    }

    return uv_run(m_main_loop, UV_RUN_DEFAULT);
}


}