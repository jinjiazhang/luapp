#ifndef _JINJIAZHANG_LISTENER_H_
#define _JINJIAZHANG_LISTENER_H_

#include "network.h"
#include "session.h"

class listener : public iobject
{
public:
    listener(network* instance, imanager* manager);
    ~listener();

    bool listen(const char* ip, int port);

    virtual void on_event(int events, int param);
    virtual void send(char* data, int len);
    virtual void close();

private:
    network* network_;
    imanager* manager_;
    socket_t listenfd_;
};

#endif