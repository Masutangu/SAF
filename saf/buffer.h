//
// Created by Masutangu on 16/8/17.
//

#ifndef SAF_BUF_H
#define SAF_BUF_H

#include <cstddef>

namespace saf {

class Buffer {

public:
    Buffer(unsigned capacity=1):m_buf(NULL), m_capacity(capacity), m_len(0) {}
    char* data() const { return m_buf; }
    unsigned int len() { return m_len; }
    void append(const char* data, unsigned int len);
    void clear() { m_len = 0; }
    ~Buffer();


private:
    char* m_buf;
    unsigned int m_len;
    unsigned int m_capacity;
};

}



#endif //SAF_BUF_H
