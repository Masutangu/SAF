//
// Created by Masutangu on 16/8/9.
//

#include "saf/header.h"

#include <cstring>

using namespace saf;

const int BUF_SIZE = 1024;

class myMsg: public Msg {
public:
    char readbuf[BUF_SIZE];
    char writebuf[BUF_SIZE];
};

class myAction: public Action {
public:
    int prepareProcess(char* buf, unsigned int& len, Msg* msg);
    int afterProcess(char* buf, unsigned int len, Msg* msg);
};

int myAction::prepareProcess(char* buf, unsigned int& len, Msg* msg) {
    myMsg* my_msg = static_cast<myMsg*> (msg);
    printf("myAction prepareProcess, data: %s\n", my_msg->readbuf);
    if (len >= BUF_SIZE) {
        memcpy(buf, my_msg->readbuf, BUF_SIZE);
        return 0;
    } else {
        len = BUF_SIZE;
        return BUF_SIZE;
    }

}

int myAction::afterProcess(char* buf, unsigned int len, Msg* msg) {
    printf("myAction afterProcess: %s\n", buf);
    myMsg* my_msg = static_cast<myMsg*> (msg);
    memcpy(my_msg->writebuf, buf, len < BUF_SIZE ? len:BUF_SIZE);
    return 0;
}

class myState1: public State {
public:
    myState1();
};

myState1::myState1() {
    myAction* action = new myAction;
    action->setActionInfo("127.0.0.1", 7000, 0); //设置action的ip和端口
    addAction(action);
}

class myHandler: public Handler {
public:
    myHandler();
    Msg* createMsg();
    int prepareProcess(char* buf, unsigned int len, Msg* msg);
    int afterProcess(char* buf, unsigned int& len, Msg* msg);

};

Msg* myHandler::createMsg() {
    return new myMsg();
}

myHandler::myHandler() {
    myState1* state1 = new myState1();
    addState(state1);

}

int myHandler::prepareProcess(char* buf, unsigned int len, Msg* msg) {
    printf("handler: prepareProcess len=%d\n", len);
    myMsg* my_msg = static_cast<myMsg*> (msg);
    memcpy(my_msg->readbuf, buf, len);
    return 0;
}

int myHandler::afterProcess(char* buf, unsigned int& len, Msg* msg) {
    myMsg* my_msg = static_cast<myMsg*> (msg);
    if (len >= 1024) {
        memcpy(buf, my_msg->writebuf, 1024);
        return 0;
    } else {
        len = 1024;
        return 1;
    }
}

int parseReq(char* buf, unsigned int len) {
    return 1; // 该请求的类型为 1，由 myHandler 处理
}

int main() {
    REGISTER_HANDLER(1, myHandler);  // 请求类型为 1 的由 myHandler 类处理
    REGISTER_HEADER_PARSER(parseReq); // 请求包头由 parseReq 函数解析

    AsyncServer server = AsyncServer();
    server.setBindAddress("0.0.0.0", 8000); // 监听 8000 端口
    server.run();
}