#ifndef _JINJIAZHANG_TCP_PROXY_H_
#define _JINJIAZHANG_TCP_PROXY_H_

#include "gwproxy.h"
#include "protonet/inetwork.h"

class gwserver;
class tcp_proxy : public gwproxy, public imanager
{
public:
    tcp_proxy(lua_State* L);
    virtual ~tcp_proxy();

    virtual bool init(gwserver* server, proxy_param param);
    virtual int  update();
    virtual void destory();
    virtual void start_session(int connid, svrid_t svrid);
    virtual void stop_session(int connid);
    virtual void send(int connid, const void* data, int len);

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    inetwork* network_;
    int number_;
};

#endif