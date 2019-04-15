#ifndef _JINJIAZHANG_ROUTERMGR_H_
#define _JINJIAZHANG_ROUTERMGR_H_

#include "rtstruct.h"
#include "lualib/lobject.h"

struct inetwork;
class routermgr : public lobject
{
public:
    routermgr(lua_State* L, inetwork* network, svrid_t svrid);
    ~routermgr();

    inetwork* network();
    int listen(lua_State* L);
    int connect(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    svrid_t svrid_;
    inetwork* network_;
};

#endif