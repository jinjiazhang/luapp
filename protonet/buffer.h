#ifndef _JINJIAZHANG_BUFFER_H_
#define _JINJIAZHANG_BUFFER_H_

#include "platform.h"

class buffer
{
public:
    buffer();
    ~buffer();
    
    char* data();
    int size();

    char* tail();
    int space();
    
    void prepare();
    bool expand(int size);
    bool push_data(char* data, int len);
    bool push_data(iovec *iov, int cnt, int ignore);
    bool pop_data(int len);
    bool pop_space(int len);
    void trim_data();

private:
    int preseted_;
    int capacity_;
    char* buffer_;
    char* begin_;
    char* end_;
};

#endif