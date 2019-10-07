#include "rtclient.h"
#include "routermgr.h"
#include "protolua/protolua.h"

rtclient::rtclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    router_ = 0;
    number_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
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
    network_ = manager->network();
    manager_ = manager;
    number_ = number;
    return true;
}

void rtclient::on_accept(int number, int error)
{
    assert(number == number_);
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", router_, error);
        return;
    }

    rtm_reg_svrid msg;
    msg.msg_type = rtm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(number_, &msg, sizeof(msg));
}

void rtclient::on_closed(int number, int error)
{
    assert(number == number_);
    luaL_callfunc(L, this, "on_closed", router_, error);
}

void rtclient::on_package(int number, char* data, int len)
{
    assert(number == number_);
    if (len < sizeof(rtm_head))
    {
        log_error("rtclient::on_package length =%d invalid", len);
        return;
    }

    rtm_head* head = (rtm_head*)data;
    switch ((rtm_type)head->msg_type)
    {
    case rtm_type::reg_svrid:
        on_reg_svrid(data, len);
        break;
    case rtm_type::remote_call:
        on_remote_call(data, len);
        break;
    case rtm_type::transmit_call:
        on_transmit_call(data, len);
        break;
    default:
        log_error("rtclient::on_package msg_type =%d invalid", head->msg_type);
        break;
    }
}

void rtclient::on_reg_svrid(char* data, int len)
{
    rtm_reg_svrid* msg = (rtm_reg_svrid*)data;
    router_ = msg->svrid;
    luaL_callfunc(L, this, "on_accept", router_, 0);
}

void rtclient::on_remote_call(char* data, int len)
{
    rtm_remote_call* msg = (rtm_remote_call*)data;
    data += sizeof(rtm_remote_call);
    len -= sizeof(rtm_remote_call);

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, msg->srcid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

void rtclient::on_transmit_call(char* data, int len)
{
    rtm_transmit_call* msg = (rtm_transmit_call*)data;
    data += sizeof(rtm_transmit_call);
    len -= sizeof(rtm_transmit_call);

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_transmit");
	luaL_pushvalue(L, msg->srcid);
	luaL_pushvalue(L, msg->roleid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

int rtclient::reg_role(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);
    roleid_t roleid = luaL_getvalue<roleid_t>(L, 2);
    rtm_reg_roleid msg;
    msg.msg_type = rtm_type::reg_roleid;
    msg.group = group;
    msg.roleid = roleid;
    network_->send(number_, &msg, sizeof(msg));
    return 0;
}

int rtclient::unreg_role(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);
    roleid_t roleid = luaL_getvalue<roleid_t>(L, 2);
    rtm_reg_roleid msg;
    msg.msg_type = rtm_type::unreg_roleid;
    msg.group = group;
    msg.roleid = roleid;
    network_->send(number_, &msg, sizeof(msg));
    return 0;
}

static char buffer[PROTO_BUFFER_SIZE];
int rtclient::call_target(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    svrid_t dstid = luaL_getvalue<int>(L, 1);

    int top = lua_gettop(L);
    size_t len = sizeof(buffer);
    if (!message_pack(L, 2, top, buffer, &len))
    {
        return 0;
    }

    rtm_forward_svrid head;
    head.msg_type = rtm_type::forward_svrid;
    head.dstid = dstid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, (int)len };
    network_->sendv(number_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int rtclient::call_transmit(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);

    luaL_checktype(L, 2, LUA_TNUMBER);
    roleid_t roleid = luaL_getvalue<roleid_t>(L, 2);

    int top = lua_gettop(L);
    size_t len = sizeof(buffer);
    if (!message_pack(L, 3, top, buffer, &len))
    {
        return 0;
    }

    rtm_forward_roleid head;
    head.msg_type = rtm_type::forward_roleid;
    head.roleid = roleid;
    head.group = group;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, (int)len };
    network_->sendv(number_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int rtclient::call_group(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);

    int top = lua_gettop(L);
    size_t len = sizeof(buffer);
    if (!message_pack(L, 2, top, buffer, &len))
    {
        return 0;
    }

    rtm_forward_group head;
    head.msg_type = rtm_type::forward_group;
    head.group = group;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, (int)len };
    network_->sendv(number_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int rtclient::call_random(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);

    int top = lua_gettop(L);
    size_t len = sizeof(buffer);
    if (!message_pack(L, 2, top, buffer, &len))
    {
        return 0;
    }

    rtm_forward_random head;
    head.msg_type = rtm_type::forward_random;
    head.group = group;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, (int)len };
    network_->sendv(number_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int rtclient::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(rtclient, reg_role)
EXPORT_OFUNC(rtclient, unreg_role)
EXPORT_OFUNC(rtclient, call_target)
EXPORT_OFUNC(rtclient, call_transmit)
EXPORT_OFUNC(rtclient, call_group)
EXPORT_OFUNC(rtclient, call_random)
EXPORT_OFUNC(rtclient, close)
const luaL_Reg* rtclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(rtclient, reg_role) },
        { IMPORT_OFUNC(rtclient, unreg_role) },
        { IMPORT_OFUNC(rtclient, call_target) },
        { IMPORT_OFUNC(rtclient, call_transmit) },
        { IMPORT_OFUNC(rtclient, call_group) },
        { IMPORT_OFUNC(rtclient, call_random) },
        { IMPORT_OFUNC(rtclient, close) },
        { NULL, NULL }
    };
    return libs;
}