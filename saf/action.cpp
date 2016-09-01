//
// Created by Masutangu on 16/8/15.
//

#include "action.h"
#include "state.h"

namespace saf {

void Action::finish(ClientContext* c_ctx) {
    m_state->finishAction(c_ctx);
}

}
