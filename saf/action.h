//
// Created by Masutangu on 16/8/15.
//

#ifndef SAF_ACTION_H
#define SAF_ACTION_H

#include "buffer.h"
#include <string>
#include <cstdio>

namespace saf {

class State;
class ClientContext;
class Msg;

class Action {

public:
    friend class State;
    friend ClientContext;

public:
    Action():m_timeout(2000) {}
    void setActionInfo(const std::string& ip, int port, int protocol) { m_ip = ip; m_port = port; m_protocol = protocol; }
    void setTimeout(unsigned int timeout) { m_timeout = timeout; }  // 设置超时时间, 单位为毫秒 <=0 为永不超时
    const char* getIp() const { return m_ip.c_str(); }
    int getPort() const { return m_port; }
    void finish(ClientContext*);


public:
    // 以下为子类必须重写的接口

    /*
     * 打包 Action 请求包到输入 buf 中, len 为输入 buf 的长度
     * 返回 0 表示打包成功, len 为实际需要的 buf 长度
     * 返回 > 表示 buf 不够, len 为实际需要的 buf 长度
     * 返回 < 0 表示失败
     */
    virtual int prepareProcess(char* buf, unsigned int& len, Msg* msg) = 0;

    /*
     * 解析 Action 回包
     * 返回 0 表示解析回包成功
     * 返回 < 0 表示出错
     * 返回 > 0 表示收包未完整
     */
    virtual int afterProcess(char* buf, unsigned int len, Msg* msg) = 0;


public:
    // 以下为子类可以重写的接口

    /*
     * action 执行出错时的回调函数, type 参照 ACTION_ERROR_ENUM 定义的枚举, ret 为错误返回码
     */
    virtual void handleError(int type, int ret) { printf("Action error type [%d] ret [%d]\n", type, ret); };


private:
    State* m_state;
    std::string m_ip;
    int m_port;
    int m_protocol;
    int m_timeout;

};

}


#endif //SAF_ACTION_H
