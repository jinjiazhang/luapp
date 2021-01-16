#ifndef _JINJIAZHANG_TCP_PROXY_H_
#define _JINJIAZHANG_TCP_PROXY_H_

#include "gwproxy.h"
#include "protonet/inetwork.h"
#include <unordered_map>

class gwserver;
class tcp_proxy : public gwproxy, public imanager
{
public:
    tcp_proxy(lua_State* L);
    virtual ~tcp_proxy();

    virtual bool init(gwserver* server, url_info* args);
    virtual int  update();
    virtual void start_session(int connid, svrid_t svrid);
    virtual void stop_session(int connid);
    virtual void raw_send(int connid, const void* data, int len);

    virtual void on_accept(int netid, int error);
    virtual void on_closed(int netid, int error);
    virtual void on_package(int netid, char* data, int len);

private:
    connid_t netid_to_connid(int netid);
    int connid_to_netid(connid_t connid);

private:
    typedef std::unordered_map<connid_t, int> connid_netid_map;
    typedef std::unordered_map<int, connid_t> netid_connid_map;

    inetwork* network_;
    int netid_;

    connid_netid_map connid_netid_map_;
    netid_connid_map netid_connid_map_;
};

#endif