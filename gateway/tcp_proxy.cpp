#include "tcp_proxy.h"
#include "gwserver.h"

tcp_proxy::tcp_proxy(lua_State* L) : gwproxy(L)
{

}

tcp_proxy::~tcp_proxy()
{
    if (number_ > 0)
    {
        network_->close(number_);
        number_ = 0;
    }
    
    num_connid_map_.clear();
    connid_num_map_.clear();
}

bool tcp_proxy::init(gwserver* server, proxy_param param)
{
    if (!gwproxy::init(server, param))
    {
        return false;
    }

    network_ = server->network();
    number_ = network_->listen(this, param.ip.c_str(), param.port);
    if (number_ <= 0)
    {
        return false;
    }
    return true;
}

int tcp_proxy::update()
{
    return 0;
}

connid_t tcp_proxy::num_to_connid(int number)
{
    num_connid_map::iterator it = num_connid_map_.find(number);
    if (it == num_connid_map_.end())
    {
        return 0;
    }
    return it->second;
}

int tcp_proxy::connid_to_num(connid_t connid)
{
    connid_num_map::iterator it = connid_num_map_.find(connid);
    if (it == connid_num_map_.end())
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
    int number = connid_to_num(connid);
    assert(number > 0);

    network_->close(number);
    num_connid_map_.erase(number);
    connid_num_map_.erase(connid);
}

void tcp_proxy::send(int connid, const void* data, int len)
{
    int number = connid_to_num(connid);
    assert(number > 0);
    network_->send(number, data, len);
}

void tcp_proxy::on_accept(int number, int error)
{
    connid_t connid = server_->gen_connid();
    num_connid_map_[number] = connid;
    connid_num_map_[connid] = number;
    gwproxy::on_accept(connid, error);
}

void tcp_proxy::on_closed(int number, int error)
{
    connid_t connid = num_to_connid(number);
    assert(connid > 0);

    gwproxy::on_closed(connid, error);
    num_connid_map_.erase(number);
    connid_num_map_.erase(connid);
}

void tcp_proxy::on_package(int number, char* data, int len)
{
    connid_t connid = num_to_connid(number);
    assert(connid > 0);

    gwproxy::on_package(connid, data, len);
}