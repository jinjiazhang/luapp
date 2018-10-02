#ifndef _JINJIAZHANG_FEPOLL_H_
#define _JINJIAZHANG_FEPOLL_H_

#include "network.h"

class fepoll : public iframe
{
#ifdef __linux__
public:
    fepoll();
    ~fepoll();

    virtual bool init();
    virtual void release();
    virtual int update(int timeout);
    virtual int add_event(iobject* object, socket_t fd, int events);
    virtual int del_event(iobject* object, socket_t fd, int events);

private:
    int epfd_;
#endif
};
#

#endif