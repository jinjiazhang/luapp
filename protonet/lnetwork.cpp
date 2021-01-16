#include "lnetwork.h"
#include "lmanager.h"
#include "protolua/message.h"

lnetwork::lnetwork(lua_State* L, inetwork* network) : lobject(L)
{
    network_ = network;
}

lnetwork::~lnetwork()
{
    network_ = nullptr;
}

inetwork* lnetwork::impl()
{
    return network_;
}

int lnetwork::listen(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    lmanager* manager = new lmanager(this->L);
    int netid = network_->listen(manager, ip, port);
    if (netid <= 0)
    {
        delete manager;
        return 0;
    }

    manager->init(this, netid);
    lua_pushlobject(L, manager);
    add_manager(manager);
    return 1;
}

int lnetwork::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    lmanager* manager = new lmanager(this->L);
    int netid = network_->connect(manager, ip, port);
    if (netid <= 0)
    {
        delete manager;
        return 0;
    }

    manager->init(this, netid);
    lua_pushlobject(L, manager);
    add_manager(manager);
    return 1;
}

int lnetwork::call(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int netid = luaL_getvalue<int>(L, 1);

    int top = lua_gettop(L);
    size_t msg_len = sizeof(msg_buf);
    if (!message_pack(L, 2, top, msg_buf, &msg_len))
    {
        return 0;
    }

    network_->send(netid, msg_buf, (int)msg_len);
    lua_pushboolean(L, true);
    return 1;
}

int lnetwork::close(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int netid = luaL_getvalue<int>(L, 1);
    network_->close(netid);

    manager_map::iterator it = managers_.find(netid);
    if (it != managers_.end())
    {
        del_manager(it->second);
    }
    return 0;
}

void lnetwork::add_manager(lmanager* manager)
{
    assert(manager->netid() != 0);
    int netid = manager->netid();
    managers_.insert(std::make_pair(netid, manager));
}

void lnetwork::del_manager(lmanager* manager)
{
    assert(manager->netid() != 0);
    managers_.erase(manager->netid());
    delete manager;
}

EXPORT_OFUNC(lnetwork, listen)
EXPORT_OFUNC(lnetwork, connect)
EXPORT_OFUNC(lnetwork, call)
EXPORT_OFUNC(lnetwork, close)
const luaL_Reg* lnetwork::get_libs()
{
    static const luaL_Reg libs[] = {
        { "listen", OFUNC(lnetwork, listen) },
        { "connect", OFUNC(lnetwork, connect) },
        { "call", OFUNC(lnetwork, call) },
        { "close", OFUNC(lnetwork, close) },
        { NULL, NULL }
    };
    return libs;
}