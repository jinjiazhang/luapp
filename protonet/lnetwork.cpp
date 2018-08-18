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
    int ret = network_->update(timeout);

    delete_set::iterator it = deletes_.begin();
    while (it != deletes_.end())
    {
        lmanager* manager = *it;
        pop_manager(manager);
        delete manager;
        deletes_.erase(it++);
    }

    return ret;
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
    push_manager(manager);
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
    push_manager(manager);
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
    manager_map::iterator it = managers_.find(number);
    if (it == managers_.end())
    {
        return 0;
    }

    network_->close(number);
    del_manager(it->second);
    return 0;
}

void lnetwork::push_manager(lmanager* manager)
{
    assert(manager->get_number() != 0);
    int number = manager->number();
    managers_.insert(std::make_pair(number, manager));
}

void lnetwork::pop_manager(lmanager* manager)
{
    assert(manager->get_number() != 0);
    int number = manager->number();
    managers_.erase(number);
}

void lnetwork::del_manager(lmanager* manager)
{
    assert(manager->get_number() != 0);
    deletes_.insert(manager);
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