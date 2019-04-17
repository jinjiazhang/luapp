#ifndef _JINJIAZHANG_GATEWAY_H_
#define _JINJIAZHANG_GATEWAY_H_

#include "gwstruct.h"
#include "lualib/lobject.h"

struct inetwork;
class gateway : public lobject
{
public:
    gateway(lua_State* L, inetwork* network, svrid_t svrid);
    ~gateway();

    inetwork* network();
    int listen(lua_State* L);
    int connect(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    svrid_t svrid_;
    inetwork* network_;
};

#endif