#ifndef _JINJIAZHANG_MYSQLMGR_H_
#define _JINJIAZHANG_MYSQLMGR_H_

#include "lualib/lobject.h"

class mysqlmgr : public lobject
{
public:
    mysqlmgr(lua_State* L);
    ~mysqlmgr();

    int connect(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:

};

#endif