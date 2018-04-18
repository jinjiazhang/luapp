#include "network.h"
#include "listener.h"
#include "connector.h"
#include "fepoll.h"
#include "fselect.h"

network::network()
{
    last_number_ = 0;
    frame_ = NULL;
}

network::~network()
{

}

void network::release()
{
    if (frame_)
    {
        frame_->release();
        frame_ = NULL;
    }
    delete this;
}

bool network::init()
{
#ifdef linux
    frame_ = new fepoll();
#else
    frame_ = new fselect();
#endif
    if (!frame_->init())
    {
        delete frame_;
        return false;
    }
    return true;
}

int network::update(int timeout)
{
    return frame_->update(timeout);
}

int network::listen(imanager* manager, const char* ip, int port)
{
    listener* object = new listener(this, manager);
    if (!object->listen(ip, port))
    {
        delete object;
        return 0;
    }
    return add_object(object);
}

int network::connect(imanager* manager, const char* ip, int port)
{
    connector* object = new connector(this, manager);
    if (!object->connect(ip, port))
    {
        delete object;
        return 0;
    }
    return add_object(object);
}

void network::send(int number, char* data, int len)
{
    iobject* object = get_object(number);
    if (!object)
    {
        return;
    }
    object->send(data, len);
}

void network::close(int number)
{
    iobject* object = get_object(number);
    if (!object)
    {
        return;
    }
    object->close();
    del_object(object);
    delete object;
}

int network::add_event(iobject* object, socket_t fd, int events)
{
    return frame_->add_event(object, fd, events);
}

int network::del_event(iobject* object, socket_t fd, int events)
{
    return frame_->del_event(object, fd, events);
}

int network::new_number()
{
    return ++last_number_;
}

int network::add_object(iobject* object)
{
    assert(object->get_number() == 0);
    int number = new_number();
    object->set_number(number);
    objects.insert(std::make_pair(number, object));
    return number;
}

int network::del_object(iobject* object)
{
    assert(object->get_number() != 0);
    objects.erase(object->get_number());
    return 0;
}

iobject* network::get_object(int number)
{
    object_map::iterator it = objects.find(number);
    if (it == objects.end())
    {
        return NULL;
    }
    return it->second;
}

inetwork* create_network()
{
    network* instance = new network();
    if (!instance->init())
    {
        delete instance;
        return NULL;
    }
    return instance;
}