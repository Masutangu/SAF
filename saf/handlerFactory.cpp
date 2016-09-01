//
// Created by Masutangu on 16/8/20.
//

#include "handlerFactory.h"

#include <cassert>
#include <cstddef>

namespace saf {

Handler* HandlerFactory::genHandlerByReq(int req) {
    std::map<int, register_func>::iterator it = m_handler_map.find(req);
    if (it != m_handler_map.end()) {
        return (Handler*) it->second();
    }
    return NULL;
}


void HandlerFactory::registerHandler(int req, register_func func) {
    assert(req > 0);
    m_handler_map.insert(std::make_pair(req, func));
}

}
