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

        int events = 0;
        if (event.filter == EVFILT_READ) events |= EVENT_READ;
        if (event.filter == EVFILT_WRITE) events |= EVENT_WRITE;
        object->on_event(events);
    }
    return count;
}

int fkqueue::add_event(iobject* object, socket_t fd, int events)
{
    int exists = object->get_events();
    int newevs = exists | events;
    int addevs = newevs & (~exists);

    struct kevent event;
    if (addevs & EVENT_READ)
    {
        EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    if (addevs & EVENT_WRITE)
    {
        EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, object);
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

    struct kevent event;
    if (delevs & EVENT_READ)
    {
        EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    if (delevs & EVENT_WRITE)
    {
        EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, object);
        kevent(handle_, &event, 1, nullptr, 0, nullptr);
    }

    object->set_events(newevs);
    return 0;
}
#endif
