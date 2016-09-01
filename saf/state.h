//
// Created by Masutangu on 16/8/15.
//

#ifndef SAF_STATE_H
#define SAF_STATE_H

#include "action.h"
#include <vector>

namespace saf {

class Handler;
class ClientContext;
class Msg;

class State {

public:
    void addAction(Action* action);
    void process(ClientContext* c_ctx);
    void finishAction(ClientContext* c_ctx);
    void setHandler(Handler* handler) { m_handler = handler; }


public:
    //以下为子类可以重写的接口

    /*
     * 执行 state 包含的 action 前, 框架会调用该函数, 在该方法添加相应的 action
     * 返回 0 表示成功
     * 返回 != 0 表示失败
     */
    virtual int prepareProcess(Msg* msg) { return 0; };
    /*
     * state 包含的 action 都执行完时, 框架会调用该函数,可以做一些后续处理工作
     * 返回 0 表示成功
     * 返回 != 0 表示失败
     */
    virtual int afterProcess(Msg* msg) { return 0; };

    /*
     * state 执行出错时的回调函数, type 参照 STATE_ERROR_ENUM 定义的枚举, ret 为相应的错误码
     */
    virtual void handleError(int type, int ret) {};

    virtual ~State();


private:
    void finish(ClientContext*);


private:
    std::vector<Action*> m_action_list;
    Handler* m_handler;
};

}

#endif //SAF_STATE_H
