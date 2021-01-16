#include "gwclient.h"
#include "gateway.h"
#include "protolua/message.h"

gwclient::gwclient(lua_State* L, svrid_t svrid) : lobject(L)
{
    svrid_ = svrid;
    gateway_ = 0;
    netid_ = 0;
    network_ = nullptr;
    manager_ = nullptr;
}

gwclient::~gwclient()
{

}

int gwclient::netid()
{
    return netid_;
}

bool gwclient::init(gateway* manager, int netid)
{
    network_ = manager->network();
    manager_ = manager;
    netid_ = netid;
    return true;
}

void gwclient::on_accept(int netid, int error)
{
    assert(netid == netid_);
    if (error != 0)
    {
        luaL_callfunc(L, this, "on_accept", gateway_, error);
        return;
    }

    gwm_reg_svrid msg;
    msg.msg_type = gwm_type::reg_svrid;
    msg.svrid = svrid_;
    network_->send(netid_, &msg, sizeof(msg));
}

void gwclient::on_closed(int netid, int error)
{
    assert(netid == netid_);
    luaL_callfunc(L, this, "on_closed", gateway_, error);
}

void gwclient::on_package(int netid, char* data, int len)
{
    assert(netid == netid_);
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
    case gwm_type::remote_call:
        on_remote_call(data, len);
        break;
    case gwm_type::session_start:
        on_session_start(data, len);
        break;
    case gwm_type::session_stop:
        on_session_stop(data, len);
        break;
    case gwm_type::transmit_data:
        on_transmit_data(data, len);
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

void gwclient::on_remote_call(char* data, int len)
{
    gwm_remote_call* msg = (gwm_remote_call*)data;
    assert(msg->srcid == gateway_);

    data += sizeof(gwm_remote_call);
    len -= sizeof(gwm_remote_call);

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

void gwclient::on_session_start(char* data, int len)
{
    gwm_session_start* msg = (gwm_session_start*)data;
    data += sizeof(gwm_session_start);
    len -= sizeof(gwm_session_start);

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_start");
    luaL_pushvalue(L, msg->connid);
    lua_pushlstring(L, data, len);
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

void gwclient::on_session_stop(char* data, int len)
{
    gwm_session_stop* msg = (gwm_session_stop*)data;
    data += sizeof(gwm_session_stop);
    len -= sizeof(gwm_session_stop);

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_stop");
    luaL_pushvalue(L, msg->connid);
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

void gwclient::on_transmit_data(char* data, int len)
{
    gwm_transmit_data* msg = (gwm_transmit_data*)data;
    data += sizeof(gwm_transmit_data);
    len -= sizeof(gwm_transmit_data);

    int top = lua_gettop(L);
    luaL_pushfunc(L, this, "on_transmit");
    luaL_pushvalue(L, msg->connid);

    if (!message_unpack(L, data, len))
    {
        lua_settop(L, top);
        return;
    }
    int nargs = lua_gettop(L) - top - 1;
    luaL_safecall(L, nargs, 0);
}

int gwclient::close(lua_State* L)
{
    return 0;
}

int gwclient::start(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    connid_t connid = luaL_getvalue<connid_t>(L, 1);
    gwm_start_session msg;
    msg.msg_type = gwm_type::start_session;
    msg.connid = connid;
    network_->send(netid_, &msg, sizeof(msg));
    return 0;
}

int gwclient::stop(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    connid_t connid = luaL_getvalue<connid_t>(L, 1);
    gwm_stop_session msg;
    msg.msg_type = gwm_type::stop_session;
    msg.connid = connid;
    network_->send(netid_, &msg, sizeof(msg));
    return 0;
}

int gwclient::transmit(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    connid_t connid = luaL_getvalue<connid_t>(L, 1);

    int top = lua_gettop(L);
    size_t msg_len = sizeof(msg_buf);
    if (!message_pack(L, 2, top, msg_buf, &msg_len))
    {
        return 0;
    }

    gwm_transmit_data head;
    head.msg_type = gwm_type::transmit_data;
    head.connid = connid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &msg_buf, (int)msg_len };
    network_->sendv(netid_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int gwclient::broadcast(lua_State* L)
{
    int top = lua_gettop(L);
    size_t len = sizeof(msg_buf);
    if (!message_pack(L, 1, top, msg_buf, &len))
    {
        return 0;
    }

    gwm_broadcast_data head;
    head.msg_type = gwm_type::broadcast_data;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { &msg_buf, (int)len };
    network_->sendv(netid_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

int gwclient::multicast(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    int count = (int)luaL_len(L, 1);

    std::vector<connid_t> connids(count);
    for (int i = 0; i < count; i++)
    {
        lua_geti(L, 1, i + 1);
        luaL_checktype(L, -1, LUA_TNUMBER);
        connids[i] = luaL_getvalue<connid_t>(L, -1);
        lua_pop(L, 1);
    }

    int top = lua_gettop(L);
    size_t len = sizeof(msg_buf);
    if (!message_pack(L, 2, top, msg_buf, &len))
    {
        return 0;
    }

    gwm_multicast_data head;
    head.msg_type = gwm_type::multicast_data;
    head.count = count;

    iobuf bufs[3];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { (char*)connids.data(), count * (int)sizeof(connid_t) };
    bufs[2] = { &msg_buf, (int)len };
    network_->sendv(netid_, bufs, 2);

    lua_pushboolean(L, true);
    return 1;
}

EXPORT_OFUNC(gwclient, close)
EXPORT_OFUNC(gwclient, start)
EXPORT_OFUNC(gwclient, stop)
EXPORT_OFUNC(gwclient, transmit)
EXPORT_OFUNC(gwclient, broadcast)
const luaL_Reg* gwclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { "close", OFUNC(gwclient, close) },
        { "start", OFUNC(gwclient, start) },
        { "stop", OFUNC(gwclient, stop) },
        { "transmit", OFUNC(gwclient, transmit) },
        { "broadcast", OFUNC(gwclient, broadcast) },
        { NULL, NULL }
    };
    return libs;
}