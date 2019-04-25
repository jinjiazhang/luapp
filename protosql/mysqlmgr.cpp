#include "mysqlmgr.h"
#include "sqlclient.h"

mysqlmgr::mysqlmgr(lua_State* L) : lobject(L)
{

}

mysqlmgr::~mysqlmgr()
{
    
}

int mysqlmgr::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* host = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* user = luaL_getvalue<const char*>(L, 2);
    luaL_checktype(L, 3, LUA_TSTRING);
    const char* passwd = luaL_getvalue<const char*>(L, 3);
    luaL_checktype(L, 4, LUA_TSTRING);
    const char* db = luaL_getvalue<const char*>(L, 4);
    luaL_checktype(L, 5, LUA_TNUMBER);
    unsigned int port = luaL_getvalue<int>(L, 5);

    sqlclient* client = new sqlclient();
    client->connect(host, user, passwd, db, port);
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