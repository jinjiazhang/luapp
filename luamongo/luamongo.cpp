#include "luamongo.h"
#include "mongoclient.h"

luamongo::luamongo(lua_State* L) : lobject(L)
{

}

luamongo::~luamongo()
{

}

int luamongo::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    mongoclient* client = new mongoclient(this->L, this);

    lua_pushlobject(L, client);
    return 1;
}

EXPORT_OFUNC(luamongo, connect)
const luaL_Reg* luamongo::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(luamongo, connect) },
        { NULL, NULL }
    };
    return libs;
}