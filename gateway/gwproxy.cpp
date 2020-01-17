#include "gwproxy.h"
#include "gateway.h"
#include "protolua/protolua.h"

gwproxy::gwproxy(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    number_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
}

gwproxy::~gwproxy()
{

}

int gwproxy::number()
{
    return number_;
}

bool gwproxy::init(gateway* manager, int number)
{
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

void gwproxy::on_accept(int number, int error)
{

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