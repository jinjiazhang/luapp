#include "gateway.h"

gateway::gateway(lua_State* L, inetwork* network, svrid_t svrid) : lobject(L)
{
    network_ = network;
    svrid_ = svrid;
}

gateway::~gateway()
{
    
}

inetwork* gateway::network()
{
    return network_;
}

int gateway::listen(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    return 0;
}

int gateway::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    return 0;
}

EXPORT_OFUNC(gateway, listen)
EXPORT_OFUNC(gateway, connect)
const luaL_Reg* gateway::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(gateway, listen) },
        { IMPORT_OFUNC(gateway, connect) },
        { NULL, NULL }
    };
    return libs;
}