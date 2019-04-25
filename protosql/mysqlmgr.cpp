#include "mysqlmgr.h"

mysqlmgr::mysqlmgr(lua_State* L) : lobject(L)
{

}

mysqlmgr::~mysqlmgr()
{
    
}

int mysqlmgr::connect(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(mysqlmgr, connect)
const luaL_Reg* mysqlmgr::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(mysqlmgr, connect) },
        { NULL, NULL }
    };
    return libs;
}