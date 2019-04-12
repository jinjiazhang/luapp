#ifndef _JINJIAZHANG_NETWORK_H_
#define _JINJIAZHANG_NETWORK_H_

#include "inetwork.h"
#include "platform.h"

#define EVENT_READ        0x01
#define EVENT_WRITE       0x04

struct iobject
{
    iobject() { number_ = 0; events_ = 0; }
    virtual ~iobject() { }
    int get_number() { return number_; }
    void set_number(int number) { number_ = number; }
    int get_events() { return events_; }
    void set_events(int events) { events_ = events; }

    virtual void on_event(int events) { }
    virtual void send(const void* data, int len) { }
    virtual void sendv(iobuf bufs[], int count) { }
    virtual void close() { }

protected:
    int number_;
    int events_;
};

struct iframe
{
    virtual bool init() = 0;
    virtual void release() = 0;
    virtual int update(int timeout) = 0;
    virtual int add_event(iobject* object, socket_t fd, int events) = 0;
    virtual int del_event(iobject* object, socket_t fd, int events) = 0;
};

class network : public inetwork
{
public:
    network();
    ~network();

    virtual bool init();
    virtual int update(int timeout);
    virtual int listen(imanager* manager, const char* ip, int port);
    virtual int connect(imanager* manager, const char* ip, int port);
    virtual void send(int number, const void* data, int len);
    virtual void sendv(int number, iobuf bufs[], int count);
    virtual void close(int number);
    virtual void release();

public:
    int add_event(iobject* object, socket_t fd, int events);
    int del_event(iobject* object, socket_t fd, int events);

    int new_number();
    int add_object(iobject* object);
    int del_object(iobject* object);
    iobject* get_object(int number);

private:
    typedef std::map<int, iobject*> object_map;

    object_map objects_;
    int last_number_;
    iframe* frame_;
};

#endif