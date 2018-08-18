#ifndef _JINJIAZHANG_NETWORK_H_
#define _JINJIAZHANG_NETWORK_H_

#include "inetwork.h"
#include "platform.h"

#define EVENT_READ        0x01
#define EVENT_WRITE        0x04
#define EVENT_ERROR        0x08

struct iobject
{
    iobject() { number_ = 0; events_ = 0; }
    virtual ~iobject() { }
    int get_number() { return number_; }
    void set_number(int number) { number_ = number; }
    int get_events() { return events_; }
    void set_events(int events) { events_ = events; }

    virtual void on_event(int events, int param) = 0;
    virtual void send(char* data, int len) = 0;
    virtual void close() = 0;

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
    virtual void send(int number, char* data, int len);
    virtual void close(int number);
    virtual void release();

public:
    int add_event(iobject* object, socket_t fd, int events);
    int del_event(iobject* object, socket_t fd, int events);
    int mark_error(iobject* object, int error);

    int new_number();
    int add_object(iobject* object);
    int del_object(iobject* object);
    iobject* get_object(int number);

private:
    typedef std::map<int, iobject*> object_map;
    typedef std::map<iobject*, int> object_err;

    object_map objects_;
    object_err errors_;
    int last_number_;
    iframe* frame_;
};

#endif