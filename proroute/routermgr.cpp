#include "routermgr.h"
#include "rtclient.h"
#include "rtserver.h"

routermgr::routermgr(lua_State* L, inetwork* network, svrid_t svrid) : lobject(L)
{
    network_ = network;
    svrid_ = svrid;
}

routermgr::~routermgr()
{
    
}

int routermgr::listen(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    rtserver* server = new rtserver(this->L, svrid_);
    int number = network_->listen(server, ip, port);
    if (number <= 0)
    {
        delete server;
        return 0;
    }

    server->init(this, number);
    lua_pushlobject(L, server);
    return 1;
}

int routermgr::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    rtclient* client = new rtclient(this->L, svrid_);
    int number = network_->connect(client, ip, port);
    if (number <= 0)
    {
        delete client;
        return 0;
    }

    client->init(this, number);
    lua_pushlobject(L, client);
    return 1;
}

EXPORT_OFUNC(routermgr, listen)
EXPORT_OFUNC(routermgr, connect)
const luaL_Reg* routermgr::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(routermgr, listen) },
        { IMPORT_OFUNC(routermgr, connect) },
        { NULL, NULL }
    };
    return libs;
}