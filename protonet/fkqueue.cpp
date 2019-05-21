#include "fkqueue.h"

#ifdef __APPLE__
#include <sys/event.h>

fkqueue::fkqueue()
{
    handle_ = -1;
}

fkqueue::~fkqueue()
{
    
}

bool fkqueue::init()
{
    handle_ = kqueue();
    if (handle_ <= 0)
    {
        return false;
    }
    return true;
}

void fkqueue::release()
{
    if (handle_ >= 0)
    {
        ::close(handle_);
        handle_ = -1;
    }
    delete this;
}

int fkqueue::update(int timeout)
{
    return 0;
}

int fkqueue::add_event(iobject* object, socket_t fd, int events)
{
    return 0;
}

int fkqueue::del_event(iobject* object, socket_t fd, int events)
{
    return 0;
}
#endif
