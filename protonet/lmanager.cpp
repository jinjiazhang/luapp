#include "lmanager.h"
#include "lnetwork.h"
#include "protolua/message.h"

lmanager::lmanager(lua_State* L) : lobject(L)
{
    network_ = nullptr;
    netid_ = 0;
}

lmanager::~lmanager()
{

}

int lmanager::netid()
{
    return netid_;
}

bool lmanager::init(lnetwork* network, int netid)
{
    network_ = network;
    netid_ = netid;
    return true;
}

int lmanager::call(lua_State* L)
{
    int top = lua_gettop(L);
    size_t msg_len = sizeof(msg_buf);
    if (!message_pack(L, 1, top, msg_buf, &msg_len))
    {
        return 0;
    }

    network_->impl()->send(netid_, msg_buf, (int)msg_len);
    lua_pushboolean(L, true);
    return 1;
}

void lmanager::close(lua_State* L)
{
    network_->impl()->close(netid_);
    network_->del_manager(this);
}

void lmanager::on_accept(int netid, int error)
{
    luaL_callfunc(L, this, "on_accept", netid, error);
}

void lmanager::on_closed(int netid, int error)
{
    luaL_callfunc(L, this, "on_closed", netid, error);
    if (netid == netid_)
    {
        network_->del_manager(this);
    }
}

void lmanager::on_package(int netid, char* data, int len)
{
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, netid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

EXPORT_OFUNC(lmanager, netid)
EXPORT_OFUNC(lmanager, call)
EXPORT_OFUNC(lmanager, close)
const luaL_Reg* lmanager::get_libs()
{
    static const luaL_Reg libs[] = {
        { "netid", OFUNC(lmanager, netid) },
        { "call", OFUNC(lmanager, call) },
        { "close", OFUNC(lmanager, close) },
        { NULL, NULL }
    };
    return libs;
}