#ifndef _JINJIAZHANG_GWPROXY_H_
#define _JINJIAZHANG_GWPROXY_H_

#include "lualib/lobject.h"
#include "gwstruct.h"
#include "gwtools.h"

class gwserver;
class gwproxy : public lobject
{
public:
    gwproxy(lua_State* L);
    virtual ~gwproxy();

    virtual bool init(gwserver* server, proxy_param param);
    virtual int  update();
    virtual void destory();
    virtual void start_session(int connid, svrid_t svrid);
    virtual void stop_session(int connid);
    virtual void send(int connid, const void* data, int len);

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

protected:
    gwserver* server_;
};

#endif