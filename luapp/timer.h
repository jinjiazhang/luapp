#ifndef _JINJIAZHANG_TIMER_H_
#define _JINJIAZHANG_TIMER_H_

#include "plat.h"

class timer
{
public:
    struct callback
    {
        virtual void timeout(int tid) = 0;
    };

public:
    timer(int curtime);
    ~timer();

public:
    int update(int64_t curtime);
    int insert(int second, callback* obj);
    bool remove(int tid);
    bool change(int tid, int second);

private:

};

#endif