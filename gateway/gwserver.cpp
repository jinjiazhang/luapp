#include "gwserver.h"
#include "gateway.h"

gwserver::gwserver(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    number_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
}

gwserver::~gwserver()
{

}

int gwserver::number()
{
    return number_;
}

bool gwserver::init(gateway* manager, int number)
{
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

int gwserver::close(lua_State* L)
{
    return 0;
}


void gwserver::on_accept(int number, int error)
{

}

void gwserver::on_closed(int number, int error)
{

}

void gwserver::on_package(int number, char* data, int len)
{

}

EXPORT_OFUNC(gwserver, close)
const luaL_Reg* gwserver::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(gwserver, close) },
        { NULL, NULL }
    };
    return libs;
}