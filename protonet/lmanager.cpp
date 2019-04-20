#include "lmanager.h"
#include "lnetwork.h"
#include "protolua/protolua.h"

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
    network_->del_manager(this);
}

void lmanager::on_accept(int number, int error)
{
    luaL_callfunc(L, this, "on_accept", number, error);
}

void lmanager::on_closed(int number, int error)
{
    luaL_callfunc(L, this, "on_closed", number, error);
    if (number == number_)
    {
        network_->del_manager(this);
    }
}

void lmanager::on_package(int number, char* data, int len)
{
    std::string proto = data;
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, number);
    luaL_pushvalue(L, proto);

    const char* input = data + proto.size() + 1;
    size_t size = len - proto.size() - 1;
    if (!proto_unpack(proto.c_str(), L, input, size))
    {
        lua_settop(L, top);
        return;
    }

    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
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