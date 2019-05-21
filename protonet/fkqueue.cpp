#include "fkqueue.h"

#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>

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
    assert(handle_ > 0);
    timespec time_wait;
    time_wait.tv_sec = timeout / 1000;
    time_wait.tv_nsec = (timeout % 1000) * 1000000;

    struct kevent results[64];
    int count = kevent(handle_, nullptr, 0, results, 64, timeout >= 0 ? &time_wait : nullptr);
    for (int i = 0; i < count; i++)
    {
        struct kevent& event = results[i];
        iobject* object = (iobject*)event.udata;
        if (event.filter == EVFILT_READ)
            object->on_event(EVENT_READ);
        else if (event.filter == EVFILT_WRITE)
            object->on_event(EVENT_WRITE);
    }
    return 0;
}

int fkqueue::add_event(iobject* object, socket_t fd, int events)
{
    int exists = object->get_events();
    int newevs = exists | events;
    int addevs = newevs & (~exists);
    int flags = (exists == 0) ? EV_ADD : EV_ENABLE;

    if (addevs & EVENT_READ)
    {
        struct kevent event;
        EV_SET(&event, fd, EVFILT_READ, flags, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    if (addevs & EVENT_WRITE)
    {
        struct kevent event;
        EV_SET(&event, fd, EVFILT_WRITE, flags, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    object->set_events(newevs);
    return 0;
}

int fkqueue::del_event(iobject* object, socket_t fd, int events)
{
    int exists = object->get_events();
    int newevs = exists & (~events);
    int delevs = exists & (~newevs);
    int flags = (newevs == 0) ? EV_DELETE : EV_DISABLE;

    if (delevs & EVENT_READ)
    {
        struct kevent event;
        EV_SET(&event, fd, EVFILT_READ, flags, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    if (delevs & EVENT_WRITE)
    {
        struct kevent event;
        EV_SET(&event, fd, EVFILT_WRITE, flags, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    object->set_events(newevs);
    return 0;
}
#endif
