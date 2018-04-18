#ifndef _JINJIAZHANG_FSELECT_H_
#define _JINJIAZHANG_FSELECT_H_

#include "network.h"

class fselect : public iframe
{
#ifdef _MSC_VER
public:
    fselect();
    ~fselect();

    virtual bool init();
    virtual void release();
    virtual int update(int timeout);
    virtual int add_event(iobject* object, socket_t fd, int events);
    virtual int del_event(iobject* object, socket_t fd, int events);

    void fire_event(socket_t fd, int events);

private:
    typedef std::map<socket_t, iobject*> handler_map;
    handler_map handlers;

    int vec_max_;
    fd_set* vec_ri_;
    fd_set* vec_ro_;
    fd_set* vec_wi_;
    fd_set* vec_wo_;
    fd_set* vec_eo_;
#endif
};

#endif