#include "gwclient.h"
#include "gateway.h"

gwclient::gwclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    number_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
}

gwclient::~gwclient()
{

}

int gwclient::number()
{
    return number_;
}

bool gwclient::init(gateway* manager, int number)
{
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

int gwclient::close(lua_State* L)
{
    return 0;
}


void gwclient::on_accept(int number, int error)
{

}

void gwclient::on_closed(int number, int error)
{

}

void gwclient::on_package(int number, char* data, int len)
{

}

EXPORT_OFUNC(gwclient, close)
const luaL_Reg* gwclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(gwclient, close) },
        { NULL, NULL }
    };
    return libs;
}