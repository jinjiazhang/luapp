#include "luaredis.h"
#include "rdsclient.h"
#include "async.h"

luaredis::luaredis(lua_State* L, inetwork* network) : lobject(L)
{
    network_ = network;
}

luaredis::~luaredis()
{
    network_ = nullptr;
}

inetwork* luaredis::network()
{
    return network_;
}

int luaredis::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    redisAsyncContext* ac = redisAsyncConnect(ip, port);
    if (ac->err) {
        lua_pushnil(L);
        lua_pushstring(L, ac->errstr);
        redisAsyncFree(ac);
        return 2;
    }

    rdsclient* client = new rdsclient(this->L, this);
    if (!client->init(ac))
    {
        lua_pushnil(L);
        lua_pushstring(L, "rdsclient init false");
        redisAsyncFree(ac);
        delete client;
        return 2;
    }

    lua_pushlobject(L, client);
    return 1;
}

EXPORT_OFUNC(luaredis, connect)
const luaL_Reg* luaredis::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(luaredis, connect) },
        { NULL, NULL }
    };
    return libs;
}