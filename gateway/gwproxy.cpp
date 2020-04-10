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

void gwproxy::destory()
{

}

void gwproxy::on_accept(int number, int error)
{
    server_->reg_connid(number, this);
    luaL_callfunc(L, this, "on_accept", number, error);
}

void gwproxy::on_closed(int number, int error)
{
    luaL_callfunc(L, this, "on_closed", number, error);
    server_->unreg_connid(number);
}

void gwproxy::on_package(int number, char* data, int len)
{
    if (server_->is_accepted(number))
    {
        server_->transmit_data(number, data, len);
        return;
    }

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, number);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}