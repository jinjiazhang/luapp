#ifndef _JINJIAZHANG_CONNECTOR_H_
#define _JINJIAZHANG_CONNECTOR_H_

#include "network.h"
#include "session.h"

class connector : public session
{
public:
    connector(network* instance, imanager* manager);
    ~connector();

    bool connect(const char* ip, int port);

    virtual void on_event(int events);
    virtual void send(const void* data, int len);
    virtual void sendv(iobuf bufs[], int count);
    virtual void close();

private:
    void on_error(int error);

private:
    socket_t connectfd_;
    bool connected_;
};

#endif