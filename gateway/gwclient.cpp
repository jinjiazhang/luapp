#include "gwclient.h"
#include "gateway.h"

gwclient::gwclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    gateway_ = 0;
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

void gwclient::on_accept(int number, int error)
{
    assert(number == number_);
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", gateway_, error);
        return;
    }

    gwm_reg_svrid msg;
    msg.msg_type = gwm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(number_, &msg, sizeof(msg));
}

void gwclient::on_closed(int number, int error)
{
    assert(number == number_);
    luaL_callfunc(L, this, "on_closed", gateway_, error);
}

void gwclient::on_package(int number, char* data, int len)
{
    assert(number == number_);
    if (len < sizeof(gwm_head))
    {
        log_error("gwclient::on_package length =%d invalid", len);
        return;
    }

    gwm_head* head = (gwm_head*)data;
    switch ((gwm_type)head->msg_type)
    {
    case gwm_type::reg_svrid:
        on_reg_svrid(data, len);
        break;
    default:
        log_error("gwclient::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void gwclient::on_reg_svrid(char* data, int len)
{
    gwm_reg_svrid* msg = (gwm_reg_svrid*)data;
    gateway_ = msg->svrid;
    luaL_callfunc(L, this, "on_accept", gateway_, 0);
}

int gwclient::close(lua_State* L)
{
    return 0;
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