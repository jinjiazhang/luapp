#ifndef _JINJIAZHANG_BUFFER_H_
#define _JINJIAZHANG_BUFFER_H_

#include "platform.h"

const int min_buffer_size = 0x2000;
const int max_buffer_size = 0x800000;

class buffer
{
public:
    buffer(int init_size);
    ~buffer();
    
    char* data();
    int size();

    char* tail();
    int space();
    
    void reserve(int count);
    void shrink();

    bool push_data(char* data, int len);
    bool push_data(iovec *iov, int cnt, int ignore);
    bool pop_data(int len);
    bool pop_space(int len);
    void trim_data();

private:
    int init_size_;
    int capacity_;
    char* buffer_;
    char* begin_;
    char* end_;
};

#endif