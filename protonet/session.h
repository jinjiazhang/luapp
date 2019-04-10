#ifndef _JINJIAZHANG_SESSION_H_
#define _JINJIAZHANG_SESSION_H_

#include "network.h"
#include "buffer.h"

class session : public iobject
{
public:
    session(network* instance, imanager* manager);
    ~session();

    bool init(socket_t fd);

    virtual void on_event(int events);
    virtual void send(const void* data, int len);
    virtual void sendv(iobuf bufs[], int count);
    virtual void close();

private:
    void on_readable();
    void on_writable();
    void on_error(int error);
    void dispatch();
    void transmit(iovec* iov, int iovcnt);

protected:
    socket_t fd_;
    imanager* manager_;
    network* network_;
    bool closed_;

    buffer recvbuf_;
    buffer sendbuf_;
};

#endif