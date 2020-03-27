#ifndef _JINJIAZHANG_gwproxy_H_
#define _JINJIAZHANG_gwproxy_H_

#include "gwstruct.h"
#include "protonet/inetwork.h"
#include "lualib/lobject.h"
#include <unordered_map>

class gateway;
class gwproxy : public lobject, public imanager
{
public:
    gwproxy(lua_State* L, svrid_t svrid);
    ~gwproxy();

    int number();
    bool init(gateway* manager, int number);
    void send(int connid, const void* data, int len);
    void start_session(int connid);
    void stop_session(int connid);

    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);

private:
    svrid_t svrid_;
    int number_;
    inetwork* network_;
    gateway* manager_;
};

#endif