#include "gateway.h"
#include "gwclient.h"
#include "gwserver.h"
#include "gwconn.h"

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

    gwserver* server = new gwserver(this->L, svrid_);
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

int gateway::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    gwclient* client = new gwclient(this->L, svrid_);
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

int gateway::open(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* url = luaL_getvalue<const char*>(L, 1);

    proxy_param param;
    if (!parse_url(url, param))
    {
        return 0;
    }

    gwconn* conn = nullptr;
    switch (param.protocol)
    {
    case protocol_type::tcp:
        conn = new gwconn(this->L);
        break;
    default:
        return 0;
    }

    if (!conn->init(this, param))
    {
        delete conn;
        return 0;
    }

    lua_pushlobject(L, conn);
    return 1;
}

EXPORT_OFUNC(gateway, listen)
EXPORT_OFUNC(gateway, connect)
EXPORT_OFUNC(gateway, open)
const luaL_Reg* gateway::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(gateway, listen) },
        { IMPORT_OFUNC(gateway, connect) },
        { IMPORT_OFUNC(gateway, open) },
        { NULL, NULL }
    };
    return libs;
}