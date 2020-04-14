#ifndef _JINJIAZHANG_GWCONN_H_
#define _JINJIAZHANG_GWCONN_H_

#include "lualib/lobject.h"
#include "protonet/inetwork.h"
#include "gwtools.h"

class gateway;
class gwconn : public lobject, public imanager
{
public:
    gwconn(lua_State* L);
    virtual ~gwconn();

    int number();
    virtual bool init(gateway* gate, proxy_param param);
    int call(lua_State* L);
    void close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int connid, int error);
    virtual void on_closed(int connid, int error);
    virtual void on_package(int connid, char* data, int len);

protected:
    gateway* gateway_;
    inetwork* network_;
    int number_;
};

#endif