//
// Created by Masutangu on 16/8/20.
//

#ifndef SAF_HANDLERFACTORY_H
#define SAF_HANDLERFACTORY_H

#include "singleton.h"
#include <map>

namespace saf {

class Handler;

typedef void* (*register_func)();

class HandlerFactory {

public:
    Handler* genHandlerByReq(int req);
    void registerHandler(int req, register_func func);

private:
    std::map<int, register_func> m_handler_map;

};

#define HANDLER_FACTORY_INSTANCE (*(Singleton<HandlerFactory>::instance()))
}

#endif //SAF_HANDLERFACTORY_H
