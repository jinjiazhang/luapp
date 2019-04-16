#include "rtserver.h"
#include "routermgr.h"

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
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

void rtserver::close()
{

}

int rtserver::svrid_to_num(svrid_t svrid)
{
    svrid_num_map::iterator it = svrid_num_map_.find(svrid);
    if (it != svrid_num_map_.end())
    {
        return it->second;
    }
    return 0;
}

svrid_t rtserver::num_to_svrid(int number)
{
    num_svrid_map::iterator it = num_svrid_map_.find(number);
    if (it != num_svrid_map_.end())
    {
        return it->second;
    }
    return 0;
}

void rtserver::map_num_svrid(int number, svrid_t svrid)
{
    svrid_num_map_[svrid] = number;
    num_svrid_map_[number] = svrid;
}

void rtserver::clean_num_svrid(int number)
{
    num_svrid_map::iterator it = num_svrid_map_.find(number);
    if (it != num_svrid_map_.end())
    {
        svrid_num_map_.erase(it->second);
        num_svrid_map_.erase(it);
    }
}

void rtserver::on_accept(int number, int error)
{
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", 0, error);
        return;
    }

    rtm_reg_svrid msg;
    msg.msg_type = rtm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(number, &msg, sizeof(msg));
}

void rtserver::on_closed(int number, int error)
{
    svrid_t svrid = num_to_svrid(number);
    luaL_callfunc(L, this, "on_closed", svrid, error);
    clean_num_svrid(number);
}

void rtserver::on_package(int number, char* data, int len)
{
    if (len < sizeof(rtm_head))
    {
        log_error("rtserver::on_package length =%d invalid", len);
        return;
    }

    rtm_head* head = (rtm_head*)data;
    switch ((rtm_type)head->msg_type)
    {
    case rtm_type::reg_svrid:
        on_reg_svrid(number, data, len);
        break;
    case rtm_type::call_target:
        on_call_target(number, data, len);
        break;
    default:
        log_error("rtserver::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void rtserver::on_reg_svrid(int number, char* data, int len)
{
    rtm_reg_svrid* msg = (rtm_reg_svrid*)data;
    map_num_svrid(number, msg->svrid);
    luaL_callfunc(L, this, "on_accept", msg->svrid, 0);
}

void rtserver::on_call_target(int number, char* data, int len)
{
    rtm_call_target* msg = (rtm_call_target*)data;
    data += sizeof(rtm_call_target);
    len -= sizeof(rtm_call_target);

    int dst_num = svrid_to_num(msg->dstid);
    if (dst_num <= 0)
    {
        log_error("rtserver::on_call_target dstid =%d notfound", msg->dstid);
        return;
    }

    svrid_t scrid = num_to_svrid(number);
    if (scrid <= 0)
    {
        log_error("rtserver::on_call_target number =%d notfound", number);
        return;
    }

    rtm_call_self head;
    head.msg_type = rtm_type::call_self;
    head.srcid = scrid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };
    network_->sendv(dst_num, bufs, 2);
}

const luaL_Reg* rtserver::get_libs()
{
    static const luaL_Reg libs[] = {
        { NULL, NULL }
    };
    return libs;
}