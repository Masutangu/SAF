//
// Created by Masutangu on 16/8/9.
//

#ifndef SAF_SAFSERVER_H
#define SAF_SAFSERVER_H


extern "C" {
#include <uv.h>
}

#include "handler.h"
#include "clientContext.h"

#include <vector>
#include <string>
#include <map>

namespace saf {

class AsyncServer{
public:
    friend ClientContext;
    int run();
    void setBindAddress(std::string ip, int port) { m_ip = ip; m_port = port; }

public:
    std::string m_ip;
    int m_port;
    uv_loop_t* m_main_loop;

private:
    uv_tcp_t m_server_handle;
};

}



#endif //SAF_SAFSERVER_H
