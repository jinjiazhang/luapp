#ifndef _JINJIAZHANG_LUAMONGO_H_
#define _JINJIAZHANG_LUAMONGO_H_

#include "lualib/lobject.h"

class luamongo : public lobject
{
public:
    luamongo(lua_State* L);
    ~luamongo();

    int connect(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:

};

#endif