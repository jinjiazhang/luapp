#include "gwconn.h"
#include "gateway.h"
#include "protolua/message.h"

gwconn::gwconn(lua_State* L) : lobject(L)
{
    gateway_ = nullptr;
    network_ = nullptr;
    number_ = 0;
}

gwconn::~gwconn()
{

}

int gwconn::number()
{
    return number_;
}

bool gwconn::init(gateway* gate, proxy_param param)
{
    gateway_ = gate;
    network_ = gateway_->network();

    number_ = network_->connect(this, param.ip, param.port);
    if (number_ <= 0)
    {
        return false;
    }
    return true;
}

static char buffer[MESSAGE_BUFFER_SIZE];
int gwconn::call(lua_State* L)
{
    int top = lua_gettop(L);
    size_t len = sizeof(buffer);
    if (!message_pack(L, 1, top, buffer, &len))
    {
        return 0;
    }

    network_->send(number_, buffer, (int)len);
    lua_pushboolean(L, true);
    return 1;
}

void gwconn::close(lua_State* L)
{
    network_->close(number_);
}

void gwconn::on_accept(int connid, int error)
{
    luaL_callfunc(L, this, "on_accept", connid, error);
}

void gwconn::on_closed(int connid, int error)
{
    luaL_callfunc(L, this, "on_closed", connid, error);
}

void gwconn::on_package(int connid, char* data, int len)
{
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, connid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

EXPORT_OFUNC(gwconn, number)
EXPORT_OFUNC(gwconn, call)
EXPORT_OFUNC(gwconn, close)
const luaL_Reg* gwconn::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(gwconn, number) },
        { IMPORT_OFUNC(gwconn, call) },
        { IMPORT_OFUNC(gwconn, close) },
        { NULL, NULL }
    };
    return libs;
}