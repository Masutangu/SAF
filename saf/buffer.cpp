//
// Created by Masutangu on 16/8/17.
//

#include "buffer.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>

namespace saf {

void Buffer::append(const char* data, unsigned int len) {
    if (m_buf == NULL) {
        m_capacity = m_capacity >= len ? m_capacity : len;
        m_len = len;
        m_buf = (char*) malloc(m_capacity);
        memcpy(m_buf, data, len);
    } else if (m_len + len <= m_capacity) {
        memcpy(m_buf + m_len, data, len);
        m_len += len;
    } else {
        m_capacity *= 2;
        char* new_buf = (char*) std::realloc(m_buf, m_capacity);
        assert(new_buf);
        m_buf = new_buf;
        std::memcpy(m_buf + m_len, data, len);
        m_len += len;
    }
}

Buffer::~Buffer() {
    if (m_buf) {
        free(m_buf);
        m_buf = NULL;
    }
}

}