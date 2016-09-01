//
// Created by Masutangu on 16/8/26.
//

#ifndef SAF_UTILS_H
#define SAF_UTILS_H

extern "C" {
#include <uv.h>
}

namespace saf {

void allocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

}




#endif //SAF_UTILS_H
