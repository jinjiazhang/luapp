#include "gwserver.h"
#include "gateway.h"
#include "gwproxy.h"
#include "protolua/protolua.h"

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

inetwork* gwserver::network()
{
    return network_;
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

svrid_t gwserver::connid_to_svrid(connid_t connid)
{
    conn_svrid_map::iterator it = conn_svrid_map_.find(connid);
    if (it == conn_svrid_map_.end())
    {
        return 0;
    }
    return it->second;
}

gwproxy* gwserver::connid_to_proxy(connid_t connid)
{
    conn_proxy_map::iterator it = conn_proxy_map_.find(connid);
    if (it == conn_proxy_map_.end())
    {
        return nullptr;
    }
    return it->second;
}

void gwserver::reg_connid(connid_t connid, gwproxy* proxy)
{
    conn_proxy_map_[connid] = proxy;
}

void gwserver::unreg_connid(connid_t connid)
{
    conn_proxy_map_.erase(connid);
}

bool gwserver::is_accepted(connid_t connid)
{
    return conn_svrid_map_.find(connid) != conn_svrid_map_.end();
}

void gwserver::transmit_data(connid_t connid, char* data, int len)
{
    gwm_transmit_data head;
    head.msg_type = gwm_type::transmit_data;
    head.connid = connid;

    iobuf bufs[2];
    bufs[0] = { &head, sizeof(head) };
    bufs[1] = { data, len };

    svrid_t svrid = connid_to_svrid(connid);
    int number = svrid_to_num(svrid);
    network_->sendv(number, bufs, 2);
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
    case gwm_type::remote_call:
        on_remote_call(number, data, len);
        break;
    case gwm_type::start_session:
        on_start_session(number, data, len);
        break;
    case gwm_type::stop_session:
        on_stop_session(number, data, len);
        break;
    case gwm_type::transmit_data:
        on_transmit_data(number, data, len);
        break;
    case gwm_type::broadcast_data:
        on_broadcast_data(number, data, len);
        break;
    case gwm_type::multicast_data:
        on_multicast_data(number, data, len);
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

void gwserver::on_remote_call(int number, char* data, int len)
{
    gwm_remote_call* msg = (gwm_remote_call*)data;
    svrid_t srcid = num_to_svrid(number);
    assert(msg->srcid == srcid);

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

void gwserver::on_start_session(int number, char* data, int len)
{
    gwm_start_session* msg = (gwm_start_session*)data;
    data += sizeof(gwm_start_session);
    len -= sizeof(gwm_start_session);

    assert(connid_to_svrid(msg->connid) == num_to_svrid(number));
    gwproxy* proxy = connid_to_proxy(msg->connid);
    if (proxy == nullptr)
    {
        return;
    }
    proxy->start_session(msg->connid);
}

void gwserver::on_stop_session(int number, char* data, int len)
{
    gwm_stop_session* msg = (gwm_stop_session*)data;
    data += sizeof(gwm_stop_session);
    len -= sizeof(gwm_stop_session);

    assert(connid_to_svrid(msg->connid) == num_to_svrid(number));
    gwproxy* proxy = connid_to_proxy(msg->connid);
    if (proxy == nullptr)
    {
        return;
    }

    proxy->stop_session(msg->connid);
    conn_svrid_map_.erase(msg->connid);
    conn_proxy_map_.erase(msg->connid);
}

void gwserver::on_transmit_data(int number, char* data, int len)
{
    gwm_transmit_data* msg = (gwm_transmit_data*)data;
    data += sizeof(gwm_transmit_data);
    len -= sizeof(gwm_transmit_data);

    assert(connid_to_svrid(msg->connid) == num_to_svrid(number));
    gwproxy* proxy = connid_to_proxy(msg->connid);
    if (proxy == nullptr)
    {
        // log_warn
        return;
    }
    proxy->send(msg->connid, data, len);
}

void gwserver::on_broadcast_data(int number, char* data, int len)
{
    gwm_broadcast_data* msg = (gwm_broadcast_data*)data;
    data += sizeof(gwm_broadcast_data);
    len -= sizeof(gwm_broadcast_data);

    svrid_t svrid = num_to_svrid(number);
    conn_svrid_map::iterator it = conn_svrid_map_.begin();
    for (; it != conn_svrid_map_.end(); ++it)
    {
        if (it->second != svrid)
        {
            continue;
        }

        connid_t connid = it->first;
        gwproxy* proxy = connid_to_proxy(connid);
        if (proxy == nullptr)
        {
            // log_warn
            continue;
        }
        proxy->send(connid, data, len);
    }   
}

void gwserver::on_multicast_data(int number, char* data, int len)
{
    gwm_multicast_data* msg = (gwm_multicast_data*)data;
    data += sizeof(gwm_multicast_data);
    len -= sizeof(gwm_multicast_data);

    connid_t* connids = (connid_t*)data;
    data += sizeof(msg->count * sizeof(connid_t));
    len -= sizeof(msg->count * sizeof(connid_t));

    for (int i = 0; i < msg->count; i++)
    {
        assert(connid_to_svrid(connids[i]) == num_to_svrid(number));
        gwproxy* proxy = connid_to_proxy(connids[i]);
        if (proxy == nullptr)
        {
            // log_warn
            continue;
        }
        proxy->send(connids[i], data, len);
    }
}

int gwserver::open(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* ip = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int port = luaL_getvalue<int>(L, 2);

    gwproxy* proxy = new gwproxy(this->L, svrid_);
    int number = network_->listen(proxy, ip, port);
    if (number <= 0)
    {
        delete proxy;
        return 0;
    }

    proxy->init(this, number);
    lua_pushlobject(L, proxy);
    return 1;
}

int gwserver::close(lua_State* L)
{
    return 0;
}

int gwserver::start(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    luaL_checktype(L, 2, LUA_TNUMBER);
    connid_t connid = luaL_getvalue<connid_t>(L, 1);
    svrid_t svrid = luaL_getvalue<svrid_t>(L, 2);
    
    assert(connid_to_svrid(connid) == 0);
    int number = svrid_to_num(svrid);
    if (number == 0)
    {
        // log_error
        return 0;
    }

    conn_svrid_map_[connid] = svrid;

    gwm_session_start msg;
    msg.msg_type = gwm_type::session_start;
    msg.connid = connid;
    network_->send(number, &msg, sizeof(msg));
    return 0;
}

int gwserver::stop(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    connid_t connid = luaL_getvalue<connid_t>(L, 1);
    svrid_t svrid = luaL_getvalue<svrid_t>(L, 2);

    assert(connid_to_svrid(connid) == svrid);
    int number = svrid_to_num(svrid);
    if (number == 0)
    {
        // log_error
        return 0;
    }

    conn_svrid_map_.erase(connid);

    gwm_session_stop msg;
    msg.msg_type = gwm_type::session_stop;
    msg.connid = connid;
    network_->send(number, &msg, sizeof(msg));
    return 0;
}

EXPORT_OFUNC(gwserver, open)
EXPORT_OFUNC(gwserver, close)
EXPORT_OFUNC(gwserver, start)
EXPORT_OFUNC(gwserver, stop)
const luaL_Reg* gwserver::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(gwserver, open) },
    	{ IMPORT_OFUNC(gwserver, close) },
        { IMPORT_OFUNC(gwserver, start) },
        { IMPORT_OFUNC(gwserver, stop) },
        { NULL, NULL }
    };
    return libs;
}