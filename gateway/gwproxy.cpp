#include "gwproxy.h"
#include "gwserver.h"
#include "protolua/message.h"

gwproxy::gwproxy(lua_State* L) : lobject(L)
{
    server_ = nullptr;
}

gwproxy::~gwproxy()
{

}

bool gwproxy::init(gwserver* server, proxy_param param)
{
    server_ = server;
    return true;
}

int gwproxy::update()
{
    return 0;
}

void gwproxy::start_session(int connid, svrid_t svrid)
{

}

void gwproxy::stop_session(int connid)
{

}

void gwproxy::send(int connid, const void* data, int len)
{

}

void gwproxy::on_accept(int connid, int error)
{
    server_->reg_connid(connid, this);
    luaL_callfunc(L, this, "on_accept", connid, error);
}

void gwproxy::on_closed(int connid, int error)
{
    luaL_callfunc(L, this, "on_closed", connid, error);
    server_->unreg_connid(connid);
}

void gwproxy::on_package(int connid, char* data, int len)
{
    if (server_->is_accepted(connid))
    {
        server_->transmit_data(connid, data, len);
        return;
    }

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, connid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}