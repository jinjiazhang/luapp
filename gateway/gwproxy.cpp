#include "gwproxy.h"
#include "gwserver.h"
#include "protolua/protolua.h"

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

}

void gwproxy::on_accept(int number, int error)
{
    luaL_callfunc(L, this, "on_accept", number, error);
}

void gwproxy::on_closed(int number, int error)
{

}

void gwproxy::on_package(int number, char* data, int len)
{

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