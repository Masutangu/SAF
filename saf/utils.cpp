//
// Created by Masutangu on 16/8/26.
//

#include "utils.h"

namespace saf {

void allocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
}

}