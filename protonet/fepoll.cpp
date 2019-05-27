#include "fepoll.h"

#ifdef __linux__
#include <sys/epoll.h>

fepoll::fepoll()
{
    epfd_ = -1;
}

fepoll::~fepoll()
{
    
}

bool fepoll::init()
{
    epfd_ = epoll_create(256);
    if (epfd_ <= 0)
    {
        return false;
    }
    return true;
}

void fepoll::release()
{
    if (epfd_ >= 0)
    {
        ::close(epfd_);
        epfd_ = -1;
    }
    delete this;
}

int fepoll::update(int timeout)
{
    assert(epfd_ > 0);
    epoll_event results[64];
    int count = epoll_wait(epfd_, results, 64, timeout);
    for (int i = 0; i < count; i++)
    {
        unsigned int events = results[i].events;
        iobject* object = (iobject*)results[i].data.ptr;
        object->on_event(events);
    }
    return count;
}

int fepoll::add_event(iobject* object, socket_t fd, int events)
{
    int exists = object->get_events();
    int newevs = exists | events;
    int op = (exists == 0) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    epoll_event event;
    event.data.ptr = object;
    event.events = newevs;
    int ret = epoll_ctl(epfd_, op, fd, &event);
    if (ret != 0) {
        return ret;
    }

    object->set_events(newevs);
    return 0;
}

int fepoll::del_event(iobject* object, socket_t fd, int events)
{
    int exists = object->get_events();
    int newevs = exists & (~events);
    int op = (newevs == 0) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

    epoll_event event;
    event.data.ptr = object;
    event.events = newevs;
    int ret = epoll_ctl(epfd_, op, fd, &event);
    if (ret != 0) {
        return ret;
    }

    object->set_events(newevs);
    return 0;
}
#endif
