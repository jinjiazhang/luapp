#include "rtserver.h"
#include "routermgr.h"
#include "protolua/message.h"

rtserver::rtserver(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    netid_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
}

rtserver::~rtserver()
{
    
}

int rtserver::netid()
{
    return netid_;
}

bool rtserver::init(routermgr* manager, int netid)
{
    network_ = manager->network();
    manager_ = manager;
    netid_ = netid;
    return true;
}

int rtserver::svrid_to_netid(svrid_t svrid)
{
    svrid_netid_map::iterator it = svrid_netid_map_.find(svrid);
    if (it == svrid_netid_map_.end())
    {
        return 0;
    }
    return it->second;
}

svrid_t rtserver::netid_to_svrid(int netid)
{
    netid_svrid_map::iterator it = netid_svrid_map_.find(netid);
    if (it == netid_svrid_map_.end())
    {
        return 0;
    }
    return it->second;
}

int rtserver::roleid_to_netid(roleid_t roleid, group_t group)
{
    transmit_netid_map::iterator it = transmit_netid_map_.find(group);
    if (it == transmit_netid_map_.end())
    {
        return 0;
    }

    roleid_netid_map::iterator it2 = it->second.find(roleid);
    if (it2 == it->second.end())
    {
        return 0;
    }
    return it2->second;
}

void rtserver::on_accept(int netid, int error)
{
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", 0, error);
        return;
    }

    rtm_reg_svrid msg;
    msg.msg_type = rtm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(netid, &msg, sizeof(msg));
}

void rtserver::on_closed(int netid, int error)
{
    svrid_t svrid = netid_to_svrid(netid);
    luaL_callfunc(L, this, "on_closed", svrid, error);

    netid_svrid_map::iterator it = netid_svrid_map_.find(netid);
    if (it != netid_svrid_map_.end())
    {
        svrid_netid_map_.erase(it->second);
        netid_svrid_map_.erase(it);
    }
}

void rtserver::on_package(int netid, char* data, int len)
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
        on_reg_svrid(netid, data, len);
        break;
    case rtm_type::reg_roleid:
        on_reg_roleid(netid, data, len);
        break;
    case rtm_type::unreg_roleid:
        on_unreg_roleid(netid, data, len);
        break;
    case rtm_type::forward_svrid:
        on_forward_svrid(netid, data, len);
        break;
    case rtm_type::forward_roleid:
        on_forward_roleid(netid, data, len);
        break;
    case rtm_type::forward_group:
        on_forward_group(netid, data, len);
        break;
    case rtm_type::forward_random:
        on_forward_random(netid, data, len);
        break;
    default:
        log_error("rtserver::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void rtserver::on_reg_svrid(int netid, char* data, int len)
{
    rtm_reg_svrid* msg = (rtm_reg_svrid*)data;
    svrid_netid_map_[msg->svrid] = netid;
    netid_svrid_map_[netid] = msg->svrid;
    luaL_callfunc(L, this, "on_accept", msg->svrid, 0);
}

void rtserver::on_reg_roleid(int netid, char* data, int len)
{
    rtm_reg_roleid* msg = (rtm_reg_roleid*)data;
    transmit_netid_map_[msg->group][msg->roleid] = netid;
}

void rtserver::on_unreg_roleid(int netid, char* data, int len)
{
    rtm_unreg_roleid* msg = (rtm_unreg_roleid*)data;
    transmit_netid_map_[msg->group].erase(msg->roleid);
}

void rtserver::on_call_server(int netid, char* data, int len)
{
    svrid_t srcid = netid_to_svrid(netid);
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, srcid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

void rtserver::on_forward_svrid(int netid, char* data, int len)
{
    rtm_forward_svrid* msg = (rtm_forward_svrid*)data;
    data += sizeof(rtm_forward_svrid);
    len -= sizeof(rtm_forward_svrid);

    if (msg->dstid == svrid_)
    {
        on_call_server(netid, data, len);
        return;
    }

    int dst_netid = svrid_to_netid(msg->dstid);
    if (dst_netid <= 0)
    {
        log_error("rtserver::on_forward_svrid dstid =%d notfound", msg->dstid);
        return;
    }

    svrid_t scrid = netid_to_svrid(netid);
    if (scrid <= 0)
    {
        log_error("rtserver::on_forward_svrid netid =%d notfound", netid);
        return;
    }

    rtm_remote_call head;
    head.msg_type = rtm_type::remote_call;
    head.srcid = scrid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };
    network_->sendv(dst_netid, bufs, 2);
}

void rtserver::on_forward_roleid(int netid, char* data, int len)
{
    rtm_forward_roleid* msg = (rtm_forward_roleid*)data;
    data += sizeof(rtm_forward_roleid);
    len -= sizeof(rtm_forward_roleid);

    int dst_netid = roleid_to_netid(msg->roleid, msg->group);
    if (dst_netid <= 0)
    {
        log_error("rtserver::on_forward_roleid roleid =%lld notfound", msg->roleid);
        return;
    }

    rtm_transmit_call head;
    head.msg_type = rtm_type::transmit_call;
    head.roleid = msg->roleid;
    head.srcid = netid_to_svrid(netid);

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };
    network_->sendv(dst_netid, bufs, 2);
}

void rtserver::on_forward_group(int netid, char* data, int len)
{
    rtm_forward_group* msg = (rtm_forward_group*)data;
    data += sizeof(rtm_forward_group);
    len -= sizeof(rtm_forward_group);

    svrid_t scrid = netid_to_svrid(netid);
    if (scrid <= 0)
    {
        log_error("rtserver::on_forward_group netid =%d notfound", netid);
        return;
    }

    group_svrids_map::iterator it = group_svrids_map_.find(msg->group);
    if (it == group_svrids_map_.end())
    {
        log_error("rtserver::on_forward_group group =%d notfound", msg->group);
        return;
    }

    rtm_remote_call head;
    head.msg_type = rtm_type::remote_call;
    head.srcid = scrid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };

    for (svrid_t dstid : it->second)
    {
        int dst_netid = svrid_to_netid(dstid);
        if (dst_netid <= 0)
        {
            log_error("rtserver::on_forward_group dstid =%d notfound", dstid);
            continue;
        }
        network_->sendv(dst_netid, bufs, 2);
    }
}

void rtserver::on_forward_random(int netid, char* data, int len)
{
    rtm_forward_random* msg = (rtm_forward_random*)data;
    data += sizeof(rtm_forward_random);
    len -= sizeof(rtm_forward_random);

    svrid_t scrid = netid_to_svrid(netid);
    if (scrid <= 0)
    {
        log_error("rtserver::on_forward_random netid =%d notfound", netid);
        return;
    }

    group_svrids_map::iterator it = group_svrids_map_.find(msg->group);
    if (it == group_svrids_map_.end())
    {
        log_error("rtserver::on_forward_random group =%d notfound", msg->group);
        return;
    }

    int index = rand() % it->second.size();
    svrid_list::iterator select = it->second.begin();
    std::advance(select, index);
    svrid_t dstid = *select;

    int dst_netid = svrid_to_netid(dstid);
    if (dst_netid <= 0)
    {
        log_error("rtserver::on_forward_random dstid =%d notfound", dstid);
        return;
    }

    rtm_remote_call head;
    head.msg_type = rtm_type::remote_call;
    head.srcid = scrid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };
    network_->sendv(dst_netid, bufs, 2);
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

int rtserver::call_target(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    svrid_t dstid = luaL_getvalue<int>(L, 1);
    int dst_netid = svrid_to_netid(dstid);
    if (dst_netid <= 0)
    {
        log_error("rtserver::call_target dstid =%d notfound", dstid);
        return 0;
    }

    int top = lua_gettop(L);
    size_t msg_len = sizeof(msg_buf);
    if (!message_pack(L, 2, top, msg_buf, &msg_len))
    {
        return 0;
    }

    rtm_remote_call head;
    head.msg_type = rtm_type::remote_call;
    head.srcid = svrid_;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { msg_buf, (int)msg_len };
    network_->sendv(dst_netid, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int rtserver::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(rtserver, set_group)
EXPORT_OFUNC(rtserver, call_target)
EXPORT_OFUNC(rtserver, close)
const luaL_Reg* rtserver::get_libs()
{
    static const luaL_Reg libs[] = {
        { "set_group", OFUNC(rtserver, set_group) },
        { "call_target", OFUNC(rtserver, call_target) },
        { "close", OFUNC(rtserver, close) },
        { NULL, NULL }
    };
    return libs;
}