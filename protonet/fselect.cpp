#include "fselect.h"

#ifdef _MSC_VER
static fd_set* fd_set_malloc(int amount)
{
    size_t size = sizeof(fd_set) + (amount-FD_SETSIZE) * sizeof(socket_t);
    return (fd_set*)malloc(size);
}

static fd_set* fd_set_realloc(fd_set* src, int amount)
{
    size_t size = sizeof(fd_set) + (amount-FD_SETSIZE) * sizeof(socket_t);
    return (fd_set*)realloc(src, size);
}

static void fd_set_copy(fd_set* dest, fd_set* src)
{
    dest->fd_count = src->fd_count;
    memcpy(dest->fd_array, src->fd_array, src->fd_count * (sizeof(socket_t)));
}

fselect::fselect()
{
    vec_max_ = 0;
    vec_ri_ = NULL;
    vec_ro_ = NULL;
    vec_wi_ = NULL;
    vec_wo_ = NULL;
    vec_eo_ = NULL;
}

fselect::~fselect()
{
    if (vec_ri_) free(vec_ri_);
    if (vec_ro_) free(vec_ro_);
    if (vec_wi_) free(vec_wi_);
    if (vec_wo_) free(vec_wo_);
    if (vec_eo_) free(vec_eo_);
}

bool fselect::init()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        WSACleanup();
        return false;
    }

    vec_max_ = FD_SETSIZE;
    vec_ri_ = fd_set_malloc(vec_max_); FD_ZERO(vec_ri_);
    vec_ro_ = fd_set_malloc(vec_max_); FD_ZERO(vec_ro_);
    vec_wi_ = fd_set_malloc(vec_max_); FD_ZERO(vec_wi_);
    vec_wo_ = fd_set_malloc(vec_max_); FD_ZERO(vec_wo_);
    vec_eo_ = fd_set_malloc(vec_max_); FD_ZERO(vec_eo_);
    return true;
}

void fselect::release()
{
    WSACleanup();
    delete this;
}

int fselect::update(int timeout)
{  
    timeval tv;
    timeval* ptv = NULL;
    if (timeout >= 0)
    {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = timeout % 1000;
        ptv = &tv;
    }

    fd_set_copy(vec_ro_, vec_ri_);
    fd_set_copy(vec_wo_, vec_wi_);
    fd_set_copy(vec_eo_, vec_wi_);

    if (select(0, vec_ro_, vec_wo_, vec_eo_, ptv) < 0)
    {
        return -1;
    }

    for (u_int i = 0; i < vec_ro_->fd_count; i++)
    {
        fire_event(vec_ro_->fd_array[i], EVENT_READ);
    }

    for (u_int i = 0; i < vec_wo_->fd_count; i++)
    {
        fire_event(vec_wo_->fd_array[i], EVENT_WRITE);
    }

    for (u_int i = 0; i < vec_eo_->fd_count; i++)
    {
        fire_event(vec_eo_->fd_array[i], EVENT_WRITE);
    }

    return 0;
}

int fselect::add_event(iobject* object, socket_t fd, int events)
{
    handler_map::iterator it = handlers.find(fd);
    if (it == handlers.end())
    {
        handlers.insert(std::make_pair(fd, object));
        if (vec_max_ < (int)handlers.size())
        {
            vec_max_ = handlers.size();
            vec_ri_ = fd_set_realloc(vec_ri_, vec_max_);
            vec_ro_ = fd_set_realloc(vec_ro_, vec_max_);
            vec_wi_ = fd_set_realloc(vec_wi_, vec_max_);
            vec_wo_ = fd_set_realloc(vec_wo_, vec_max_);
            vec_eo_ = fd_set_realloc(vec_eo_, vec_max_);
        }
    }
    else if (it->second != object)
    {
        assert(0);
        return -1;
    }
    
    if (events & EVENT_READ)
        FD_SET(fd, vec_ri_);
    if (events & EVENT_WRITE)
        FD_SET(fd, vec_wi_);

    int exists = object->get_events();
    object->set_events(exists | events);
    return 0;
}

int fselect::del_event(iobject* object, socket_t fd, int events)
{
    if (events & EVENT_READ)
        FD_CLR(fd, vec_ri_);
    if (events & EVENT_WRITE)
        FD_CLR(fd, vec_wi_);

    int exists = object->get_events();
    object->set_events(exists & (~events));

    if (object->get_events() == 0)
    {
        handlers.erase(fd);
    }
    return 0;
}

void fselect::fire_event(socket_t fd, int events)
{
    handler_map::iterator it = handlers.find(fd);
    if (it == handlers.end())
    {
        assert(0);
        return;
    }
    it->second->on_event(events, 0);
}
#endif