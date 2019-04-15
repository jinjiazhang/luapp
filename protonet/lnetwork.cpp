#include "lnetwork.h"
#include "lmanager.h"
#include "protolua/protolua.h"

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
    int number = network_->listen(manager, ip, port);
    if (number <= 0)
    {
        delete manager;
        return 0;
    }

    manager->init(this, number);
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
    int number = network_->connect(manager, ip, port);
    if (number <= 0)
    {
        delete manager;
        return 0;
    }

    manager->init(this, number);
    lua_pushlobject(L, manager);
    add_manager(manager);
    return 1;
}

int lnetwork::call(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int number = luaL_getvalue<int>(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    std::string proto = luaL_getvalue<std::string>(L, 2);

    static char buffer[64 * 1024];
    strcpy(buffer, proto.c_str());

    int top = lua_gettop(L);
    char* output = buffer + proto.size() + 1;
    size_t size = sizeof(buffer) - proto.size() - 1;
    if (!proto_pack(proto.c_str(), L, 3, top, output, &size))
    {
        lua_pushboolean(L, false);
        return 1;
    }

    network_->send(number, buffer, proto.size() + 1 + size);
    lua_pushboolean(L, true);
    return 1;
}

int lnetwork::close(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int number = luaL_getvalue<int>(L, 1);
    network_->close(number);

    manager_map::iterator it = managers_.find(number);
    if (it != managers_.end())
    {
        del_manager(it->second);
    }
    return 0;
}

void lnetwork::add_manager(lmanager* manager)
{
    assert(manager->number() != 0);
    int number = manager->number();
    managers_.insert(std::make_pair(number, manager));
}

void lnetwork::del_manager(lmanager* manager)
{
    assert(manager->number() != 0);
    managers_.erase(manager->number());
    delete manager;
}

EXPORT_OFUNC(lnetwork, listen)
EXPORT_OFUNC(lnetwork, connect)
EXPORT_OFUNC(lnetwork, call)
EXPORT_OFUNC(lnetwork, close)
const luaL_Reg* lnetwork::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(lnetwork, listen) },
        { IMPORT_OFUNC(lnetwork, connect) },
        { IMPORT_OFUNC(lnetwork, call) },
        { IMPORT_OFUNC(lnetwork, close) },
        { NULL, NULL }
    };
    return libs;
}