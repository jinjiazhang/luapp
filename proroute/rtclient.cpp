#include "rtclient.h"

rtclient::rtclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
}

rtclient::~rtclient()
{
    
}

int rtclient::number()
{
    return number_;
}

bool rtclient::init(routermgr* manager, int number)
{
    return true;
}

void rtclient::close()
{

}

void rtclient::on_accept(int number, int error)
{

}

void rtclient::on_closed(int number, int error)
{

}

void rtclient::on_package(int number, char* data, int len)
{

}

const luaL_Reg* rtclient::get_libs()
{
    return nullptr;
}