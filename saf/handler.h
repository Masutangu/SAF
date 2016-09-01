//
// Created by Masutangu on 16/8/15.
//

#ifndef SAF_HANDLER_H
#define SAF_HANDLER_H

#include "state.h"
#include "clientContext.h"
#include "handlerFactory.h"

#include <vector>

namespace saf {

#define REGISTER_HANDLER(cmd, handler_class) \
   class handler_class##Register { \
    public: \
        static void* newInstance() { \
            return Singleton<handler_class>::instance(); \
        } \
    }; \
    HANDLER_FACTORY_INSTANCE.registerHandler(cmd, handler_class##Register::newInstance);


class ClientContext;
class Msg;

class Handler {
public:
    void process(ClientContext*);
    void addState(State* state) { m_state_list.push_back(state); state->setHandler(this); }

public:
    //以下为子类必须重写的接口

    /*
     * 解析客户端请求包
     * 返回 > 0 表示收包不完整
     * 返回 0 表示解析成功
     * 返回 < 0 表示解包失败, server将会杀掉客户端连接
     */
    virtual int prepareProcess(char* buf, unsigned int len, Msg* msg) = 0;

    /*
     * 打包客户端回包到输入 buf 中,len 为输入 buf 长度
     * 返回 > 0 表示 buf 不够, len 为实际需要的 buf 长度
     * 返回 0 表示打包成功, len 为 buf 的实际长度
     * 返回 < 0 表示打包失败, server将会杀掉客户端连接
     */
    virtual int afterProcess(char* buf, unsigned int& len, Msg* msg) = 0;

    /*
     * 创建该 handler 的 msg
     */
    virtual Msg* createMsg() = 0;


public:
    //以下为子类可以重写的接口

    /*
     * handler 执行出错时的回调函数, type 参照 HANDLER_ERROR_ENUM 定义的枚举, ret 为相应的错误码
     */
    virtual void handleError(int type, int ret) { printf("handler error type [%d] ret [%d]\n", type, ret); };

    virtual ~Handler();


private:
    std::vector<State*> m_state_list;

};


}


#endif //SAF_HANDLER_H
