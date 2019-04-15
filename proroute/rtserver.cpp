#include "rtserver.h"

rtserver::rtserver(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
}

rtserver::~rtserver()
{
    
}

int rtserver::number()
{
    return number_;
}

bool rtserver::init(routermgr* manager, int number)
{
    return true;
}

void rtserver::close()
{

}

void rtserver::on_accept(int number, int error)
{

}

void rtserver::on_closed(int number, int error)
{

}

void rtserver::on_package(int number, char* data, int len)
{

}

const luaL_Reg* rtserver::get_libs()
{
    return nullptr;
}