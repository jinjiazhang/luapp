#ifndef _JINJIAZHANG_NETWORK_H_
#define _JINJIAZHANG_NETWORK_H_

#include "inetwork.h"
#include "platform.h"

#define EVENT_READ        0x01
#define EVENT_WRITE       0x04

struct iobject
{
    iobject() { netid_ = 0; events_ = 0; }
    virtual ~iobject() { }
    int get_netid() { return netid_; }
    void set_netid(int netid) { netid_ = netid; }
    int get_events() { return events_; }
    void set_events(int events) { events_ = events; }

    virtual void on_event(int events) { }
    virtual void send(const void* data, int len) { }
    virtual void sendv(iobuf bufs[], int count) { }
    virtual void close() { }

protected:
    int netid_;
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
    virtual void send(int netid, const void* data, int len);
    virtual void sendv(int netid, iobuf bufs[], int count);
    virtual void close(int netid);
    virtual void release();

public:
    int add_event(iobject* object, socket_t fd, int events);
    int del_event(iobject* object, socket_t fd, int events);

    int new_netid();
    int add_object(iobject* object);
    int del_object(iobject* object);
    iobject* get_object(int netid);

private:
    typedef std::map<int, iobject*> object_map;

    object_map objects_;
    int last_netid_;
    iframe* frame_;
};

#endif