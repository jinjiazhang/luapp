#include "lmanager.h"
#include "lnetwork.h"

lmanager::lmanager(lua_State* L) : lobject(L)
{
    network_ = nullptr;
    number_ = 0;
}

lmanager::~lmanager()
{

}

int lmanager::number()
{
    return number_;
}

bool lmanager::init(lnetwork* network, int number)
{
    network_ = network;
    number_ = number;
    return true;
}

void lmanager::close()
{
    network_->impl()->close(number_);
    network_->del_manager(number_);
}

void lmanager::on_accept(int number, int error)
{
    luaL_callfunc(L, this, "on_accept", number, error);
}

void lmanager::on_closed(int number, int error)
{
    luaL_callfunc(L, this, "on_closed", number, error);
    network_->del_manager(number);
}

void lmanager::on_package(int number, char* data, int len)
{
    luaL_callfunc(L, this, "on_package", number, std::string(data, len));
}

EXPORT_OFUNC(lmanager, number)
EXPORT_OFUNC(lmanager, close)
const luaL_Reg* lmanager::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(lmanager, number) },
        { IMPORT_OFUNC(lmanager, close) },
        { NULL, NULL }
    };
    return libs;
}