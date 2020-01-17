#include "gwaccess.h"
#include "gateway.h"
#include "protolua/protolua.h"

gwaccess::gwaccess(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    number_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
}

gwaccess::~gwaccess()
{

}

int gwaccess::number()
{
    return number_;
}

bool gwaccess::init(gateway* manager, int number)
{
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

void gwaccess::on_accept(int number, int error)
{

}

void gwaccess::on_closed(int number, int error)
{

}

void gwaccess::on_package(int number, char* data, int len)
{

}

int gwaccess::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(gwaccess, close)
const luaL_Reg* gwaccess::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(gwaccess, close) },
        { NULL, NULL }
    };
    return libs;
}