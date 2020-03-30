#include "gwproxy.h"
#include "gwserver.h"
#include "protolua/message.h"

gwproxy::gwproxy(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    number_ = 0;
    network_ = nullptr;
    server_ = nullptr;
}

gwproxy::~gwproxy()
{

}

int gwproxy::number()
{
    return number_;
}

bool gwproxy::init(gwserver* server, int number)
{
    network_ = server->network();
    server_ = server;
    number_ = number;
    return true;
}

void gwproxy::send(int connid, const void* data, int len)
{
    network_->send(connid, data, len);
}

void gwproxy::start_session(int connid)
{

}

void gwproxy::stop_session(int connid)
{
    network_->close(connid);
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

int gwproxy::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(gwproxy, close)
const luaL_Reg* gwproxy::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(gwproxy, close) },
        { NULL, NULL }
    };
    return libs;
}