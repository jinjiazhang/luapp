#ifndef _JINJIAZHANG_LUAREDIS_H_
#define _JINJIAZHANG_LUAREDIS_H_

#include "lualib/lobject.h"

struct inetwork;
class luaredis : public lobject
{
public:
    luaredis(lua_State* L, inetwork* network);
    ~luaredis();

    inetwork* network();
    int connect(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    inetwork* network_;
};

#endif