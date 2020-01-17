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

int gwserver::svrid_to_num(svrid_t svrid)
{
    svrid_num_map::iterator it = svrid_num_map_.find(svrid);
    if (it == svrid_num_map_.end())
    {
        return 0;
    }
    return it->second;
}

svrid_t gwserver::num_to_svrid(int number)
{
    num_svrid_map::iterator it = num_svrid_map_.find(number);
    if (it == num_svrid_map_.end())
    {
        return 0;
    }
    return it->second;
}

void gwserver::on_accept(int number, int error)
{
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", 0, error);
        return;
    }

    gwm_reg_svrid msg;
    msg.msg_type = gwm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(number, &msg, sizeof(msg));
}

void gwserver::on_closed(int number, int error)
{
    svrid_t svrid = num_to_svrid(number);
    luaL_callfunc(L, this, "on_closed", svrid, error);

    num_svrid_map::iterator it = num_svrid_map_.find(number);
    if (it != num_svrid_map_.end())
    {
        svrid_num_map_.erase(it->second);
        num_svrid_map_.erase(it);
    }
}

void gwserver::on_package(int number, char* data, int len)
{
    if (len < sizeof(gwm_head))
    {
        log_error("gwserver::on_package length =%d invalid", len);
        return;
    }

    gwm_head* head = (gwm_head*)data;
    switch ((gwm_type)head->msg_type)
    {
    case gwm_type::reg_svrid:
        on_reg_svrid(number, data, len);
        break;
    default:
        log_error("gwserver::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void gwserver::on_reg_svrid(int number, char* data, int len)
{
    gwm_reg_svrid* msg = (gwm_reg_svrid*)data;
    svrid_num_map_[msg->svrid] = number;
    num_svrid_map_[number] = msg->svrid;
    luaL_callfunc(L, this, "on_accept", msg->svrid, 0);
}

int gwserver::close(lua_State* L)
{
    return 0;
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