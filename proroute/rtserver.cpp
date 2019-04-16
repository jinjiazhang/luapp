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
    if (it == svrid_num_map_.end())
    {
        return 0;
    }
    return it->second;
}

svrid_t rtserver::num_to_svrid(int number)
{
    num_svrid_map::iterator it = num_svrid_map_.find(number);
    if (it == num_svrid_map_.end())
    {
        return 0;
    }
    return it->second;
}

int rtserver::roleid_to_num(roleid_t roleid)
{
    roleid_num_map::iterator it = roleid_num_map_.find(roleid);
    if (it == roleid_num_map_.end())
    {
        return 0;
    }
    return it->second;
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

    num_svrid_map::iterator it = num_svrid_map_.find(number);
    if (it != num_svrid_map_.end())
    {
        svrid_num_map_.erase(it->second);
        num_svrid_map_.erase(it);
    }
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
    case rtm_type::reg_roleid:
        on_reg_roleid(number, data, len);
        break;
    case rtm_type::unreg_roleid:
        on_unreg_roleid(number, data, len);
        break;
    case rtm_type::forward_svrid:
        on_forward_svrid(number, data, len);
        break;
    case rtm_type::forward_roleid:
        on_forward_roleid(number, data, len);
        break;
    default:
        log_error("rtserver::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void rtserver::on_reg_svrid(int number, char* data, int len)
{
    rtm_reg_svrid* msg = (rtm_reg_svrid*)data;
    svrid_num_map_[msg->svrid] = number;
    num_svrid_map_[number] = msg->svrid;
    luaL_callfunc(L, this, "on_accept", msg->svrid, 0);
}

void rtserver::on_reg_roleid(int number, char* data, int len)
{
    rtm_reg_roleid* msg = (rtm_reg_roleid*)data;
    roleid_num_map_[msg->roleid] = number;
}

void rtserver::on_unreg_roleid(int number, char* data, int len)
{
    rtm_unreg_roleid* msg = (rtm_unreg_roleid*)data;
    roleid_num_map_.erase(msg->roleid);
}

void rtserver::on_forward_svrid(int number, char* data, int len)
{
    rtm_forward_svrid* msg = (rtm_forward_svrid*)data;
    data += sizeof(rtm_forward_svrid);
    len -= sizeof(rtm_forward_svrid);

    int dst_num = svrid_to_num(msg->dstid);
    if (dst_num <= 0)
    {
        log_error("rtserver::on_forward_svrid dstid =%d notfound", msg->dstid);
        return;
    }

    svrid_t scrid = num_to_svrid(number);
    if (scrid <= 0)
    {
        log_error("rtserver::on_forward_svrid number =%d notfound", number);
        return;
    }

    rtm_remote_call head;
    head.msg_type = rtm_type::remote_call;
    head.srcid = scrid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };
    network_->sendv(dst_num, bufs, 2);
}

void rtserver::on_forward_roleid(int number, char* data, int len)
{
    rtm_forward_roleid* msg = (rtm_forward_roleid*)data;
    data += sizeof(rtm_forward_roleid);
    len -= sizeof(rtm_forward_roleid);

    int dst_num = roleid_to_num(msg->roleid);
    if (dst_num <= 0)
    {
        log_error("rtserver::on_forward_roleid roleid =%d notfound", msg->roleid);
        return;
    }

    rtm_forward_roleid head;
    head.msg_type = rtm_type::forward_roleid;
    head.roleid = msg->roleid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };
    network_->sendv(dst_num, bufs, 2);
}

int rtserver::set_group(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    svrid_t svrid = luaL_getvalue<svrid_t>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 2);

    if (group > 0)
    {
        svrid_group_map_[svrid] = group;
        group_svrids_map_[group].insert(svrid);
    }
    else
    {
        svrid_group_map::iterator it1 = svrid_group_map_.find(svrid);
        if (it1 == svrid_group_map_.end())
        {
            lua_pushboolean(L, false);
            return 1;
        }

        group = it1->second;
        svrid_group_map_.erase(it1);

        group_svrids_map::iterator it2 = group_svrids_map_.find(group);
        if (it2 == group_svrids_map_.end())
        {
            lua_pushboolean(L, false);
            return 1;
        }

        it2->second.erase(svrid);
        if (it2->second.empty())
        {
            group_svrids_map_.erase(it2);
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

EXPORT_OFUNC(rtserver, set_group)
const luaL_Reg* rtserver::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(rtserver, set_group) },
        { NULL, NULL }
    };
    return libs;
}