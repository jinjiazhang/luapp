#include "sqlpool.h"
#include "sqlclient.h"
#include "mysqlmgr.h"

using namespace google::protobuf;

sqlpool::sqlpool(lua_State* L) : lobject(L)
{
    sqlmgr_ = nullptr;
}

sqlpool::~sqlpool()
{

}

bool sqlpool::init(mysqlmgr* sqlmgr)
{
    sqlmgr_ = sqlmgr;
    return true;
}

int sqlpool::connect(lua_State* L)
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

int sqlpool::sql_select(lua_State* L)
{
    return 0;
}

int sqlpool::sql_insert(lua_State* L)
{
    return 0;
}

int sqlpool::sql_update(lua_State* L)
{
    return 0;
}

int sqlpool::sql_delete(lua_State* L)
{
    return 0;
}

int sqlpool::sql_execute(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(sqlpool, connect)
EXPORT_OFUNC(sqlpool, sql_select)
EXPORT_OFUNC(sqlpool, sql_insert)
EXPORT_OFUNC(sqlpool, sql_update)
EXPORT_OFUNC(sqlpool, sql_delete)
EXPORT_OFUNC(sqlpool, sql_execute)
const luaL_Reg* sqlpool::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(sqlpool, sql_select) },
        { IMPORT_OFUNC(sqlpool, sql_insert) },
        { IMPORT_OFUNC(sqlpool, sql_update) },
        { IMPORT_OFUNC(sqlpool, sql_delete) },
        { IMPORT_OFUNC(sqlpool, connect) },
        { IMPORT_OFUNC(sqlpool, sql_execute) },
        { NULL, NULL }
    };
    return libs;
}