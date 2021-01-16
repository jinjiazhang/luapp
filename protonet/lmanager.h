#ifndef _JINJIAZHANG_LMANAGER_H_
#define _JINJIAZHANG_LMANAGER_H_

#include "inetwork.h"
#include "lualib/lobject.h"

class lnetwork;
class lmanager : public lobject, public imanager
{
public:
    lmanager(lua_State* L);
    ~lmanager();

    int netid();
    bool init(lnetwork* network, int netid);

    int call(lua_State* L);
    void close(lua_State* L);
    virtual const luaL_Reg* get_libs();

    virtual void on_accept(int netid, int error);
    virtual void on_closed(int netid, int error);
    virtual void on_package(int netid, char* data, int len);

private:
    int netid_;
    lnetwork* network_;
};

#endif