#ifndef _JINJIAZHANG_FKQUEUE_H_
#define _JINJIAZHANG_FKQUEUE_H_

#include "network.h"

class fkqueue : public iframe
{
#ifdef __APPLE__
public:
    fkqueue();
    ~fkqueue();

    virtual bool init();
    virtual void release();
    virtual int update(int timeout);
    virtual int add_event(iobject* object, socket_t fd, int events);
    virtual int del_event(iobject* object, socket_t fd, int events);

private:
    int handle_;
#endif
};
#

#endif