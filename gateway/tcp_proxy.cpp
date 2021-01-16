#include "tcp_proxy.h"
#include "gwserver.h"

tcp_proxy::tcp_proxy(lua_State* L) : gwproxy(L)
{

}

tcp_proxy::~tcp_proxy()
{
    if (netid_ > 0)
    {
        network_->close(netid_);
        netid_ = 0;
    }
    
    netid_connid_map_.clear();
    connid_netid_map_.clear();
}

bool tcp_proxy::init(gwserver* server, url_info* args)
{
    if (!gwproxy::init(server, args))
    {
        return false;
    }

    network_ = server->network();
    netid_ = network_->listen(this, args->ip, args->port);
    if (netid_ <= 0)
    {
        return false;
    }
    return true;
}

int tcp_proxy::update()
{
    return 0;
}

connid_t tcp_proxy::netid_to_connid(int netid)
{
    netid_connid_map::iterator it = netid_connid_map_.find(netid);
    if (it == netid_connid_map_.end())
    {
        return 0;
    }
    return it->second;
}

int tcp_proxy::connid_to_netid(connid_t connid)
{
    connid_netid_map::iterator it = connid_netid_map_.find(connid);
    if (it == connid_netid_map_.end())
    {
        return 0;
    }
    return it->second;
}

void tcp_proxy::start_session(int connid, svrid_t svrid)
{

}

void tcp_proxy::stop_session(int connid)
{
    int netid = connid_to_netid(connid);
    assert(netid > 0);

    network_->close(netid);
    netid_connid_map_.erase(netid);
    connid_netid_map_.erase(connid);
    gwproxy::stop_session(connid);
}

void tcp_proxy::raw_send(int connid, const void* data, int len)
{
    int netid = connid_to_netid(connid);
    assert(netid > 0);
    network_->send(netid, data, len);
}

void tcp_proxy::on_accept(int netid, int error)
{
    connid_t connid = server_->gen_connid();
    netid_connid_map_[netid] = connid;
    connid_netid_map_[connid] = netid;
    gwproxy::on_accept(connid, error);
}

void tcp_proxy::on_closed(int netid, int error)
{
    connid_t connid = netid_to_connid(netid);
    assert(connid > 0);

    netid_connid_map_.erase(netid);
    connid_netid_map_.erase(connid);
    gwproxy::on_closed(connid, error);
}

void tcp_proxy::on_package(int netid, char* data, int len)
{
    connid_t connid = netid_to_connid(netid);
    assert(connid > 0);

    gwproxy::on_package(connid, data, len);
}