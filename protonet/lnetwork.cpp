#include "lnetwork.h"
#include "lmanager.h"

lnetwork::lnetwork(lua_State* L) : lobject(L)
{
    network_ = create_network();
}

lnetwork::~lnetwork()
{
    network_->release();
    network_ = nullptr;
}

int lnetwork::update(int timeout)
{
    return network_->update(timeout);
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
    int number = network_->listen(manager, ip, port);
    if (number <= 0)
    {
        delete manager;
        return 0;
    }

    manager->init(this, number);
    lua_pushlobject(L, manager);
    add_manager(number, manager);
    return 1;
}

int lnetwork::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    lmanager* manager = new lmanager(this->L);
    int number = network_->connect(manager, ip, port);
    if (number <= 0)
    {
        delete manager;
        return 0;
    }

    manager->init(this, number);
    lua_pushlobject(L, manager);
    add_manager(number, manager);
    return 1;
}

int lnetwork::send(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int number = luaL_getvalue<int>(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    std::string data = luaL_getvalue<std::string>(L, 2);
    network_->send(number, (char*)data.c_str(), data.size());
    return 0;
}

int lnetwork::close(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int number = luaL_getvalue<int>(L, 1);
    network_->close(number);
    del_manager(number);
    return 0;
}

void lnetwork::add_manager(int number, imanager* manager)
{
    managers_.insert(std::make_pair(number, manager));
}

void lnetwork::del_manager(int number)
{
    manager_map::iterator it = managers_.find(number);
    if (it != managers_.end())
    {
        delete it->second;
        managers_.erase(it);
    }
}

EXPORT_OFUNC(lnetwork, listen)
EXPORT_OFUNC(lnetwork, connect)
EXPORT_OFUNC(lnetwork, send)
EXPORT_OFUNC(lnetwork, close)
const luaL_Reg* lnetwork::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(lnetwork, listen) },
        { IMPORT_OFUNC(lnetwork, connect) },
        { IMPORT_OFUNC(lnetwork, send) },
        { IMPORT_OFUNC(lnetwork, close) },
        { NULL, NULL }
    };
    return libs;
}