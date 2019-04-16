#include "rtclient.h"
#include "routermgr.h"
#include "protolua/protolua.h"

rtclient::rtclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    router_ = 0;
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

void rtclient::close()
{

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
    case rtm_type::forward_roleid:
        on_forward_roleid(data, len);
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

    std::string proto = data;
    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_message");
    luaL_pushvalue(L, msg->srcid);
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

void rtclient::on_forward_roleid(char* data, int len)
{
    rtm_forward_roleid* msg = (rtm_forward_roleid*)data;
    data += sizeof(rtm_forward_roleid);
    len -= sizeof(rtm_forward_roleid);

    //TODO send to client
}

int rtclient::reg_role(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    roleid_t roleid = luaL_getvalue<roleid_t>(L, 1);
    rtm_reg_roleid msg;
    msg.msg_type = rtm_type::reg_roleid;
    msg.roleid = roleid;
    network_->send(number_, &msg, sizeof(msg));
    return 0;
}

int rtclient::unreg_role(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    roleid_t roleid = luaL_getvalue<roleid_t>(L, 1);
    rtm_unreg_roleid msg;
    msg.msg_type = rtm_type::unreg_roleid;
    msg.roleid = roleid;
    network_->send(number_, &msg, sizeof(msg));
    return 0;
}

static char buffer[64 * 1024];
static int pack_message(lua_State* L, int start, int end)
{
    luaL_checktype(L, start, LUA_TSTRING);
    std::string proto = luaL_getvalue<std::string>(L, start);
    strcpy(buffer, proto.c_str());

    char* output = buffer + proto.size() + 1;
    size_t size = sizeof(buffer) - proto.size() - 1;
    if (!proto_pack(proto.c_str(), L, start + 1, end, output, &size))
    {
        log_error("proroute::pack_message false, proto =%s", proto.c_str());
        return -1;
    }
    return size;
}

int rtclient::call_target(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    svrid_t dstid = luaL_getvalue<int>(L, 1);

    int top = lua_gettop(L);
    int len = pack_message(L, 2, top);
    if (len <= 0)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    rtm_forward_svrid head;
    head.msg_type = rtm_type::forward_svrid;
    head.dstid = dstid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, len };
    network_->sendv(number_, bufs, 2);
    lua_pushboolean(L, true);
    return 1;
}

int rtclient::call_client(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    roleid_t roleid = luaL_getvalue<roleid_t>(L, 1);

    int top = lua_gettop(L);
    int len = pack_message(L, 2, top);
    if (len <= 0)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    rtm_forward_roleid head;
    head.msg_type = rtm_type::forward_roleid;
    head.roleid = roleid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, len };
    network_->sendv(number_, bufs, 2);
    lua_pushboolean(L, true);
    return 1;
}

int rtclient::call_group(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);

    int top = lua_gettop(L);
    int len = pack_message(L, 2, top);
    if (len <= 0)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    rtm_forward_group head;
    head.msg_type = rtm_type::forward_group;
    head.group = group;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, len };
    network_->sendv(number_, bufs, 2);
    lua_pushboolean(L, true);
    return 1;
}

int rtclient::call_random(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    group_t group = luaL_getvalue<group_t>(L, 1);

    int top = lua_gettop(L);
    int len = pack_message(L, 2, top);
    if (len <= 0)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    rtm_forward_random head;
    head.msg_type = rtm_type::forward_random;
    head.group = group;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &buffer, len };
    network_->sendv(number_, bufs, 2);
    lua_pushboolean(L, true);
    return 1;
}

EXPORT_OFUNC(rtclient, reg_role)
EXPORT_OFUNC(rtclient, unreg_role)
EXPORT_OFUNC(rtclient, call_target)
EXPORT_OFUNC(rtclient, call_client)
const luaL_Reg* rtclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(rtclient, reg_role) },
        { IMPORT_OFUNC(rtclient, unreg_role) },
        { IMPORT_OFUNC(rtclient, call_target) },
        { IMPORT_OFUNC(rtclient, call_client) },
        { NULL, NULL }
    };
    return libs;
}