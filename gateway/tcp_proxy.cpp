#include "tcp_proxy.h"
#include "gwserver.h"

tcp_proxy::tcp_proxy(lua_State* L) : gwproxy(L)
{

}

tcp_proxy::~tcp_proxy()
{

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

void tcp_proxy::start_session(int connid, svrid_t svrid)
{

}

void tcp_proxy::stop_session(int connid)
{
    network_->close(connid);
}

void tcp_proxy::send(int connid, const void* data, int len)
{
    network_->send(connid, data, len);
}

void tcp_proxy::destory()
{

}

void tcp_proxy::on_accept(int number, int error)
{
    gwproxy::on_accept(number, error);
}

void tcp_proxy::on_closed(int number, int error)
{
    gwproxy::on_closed(number, error);
}

void tcp_proxy::on_package(int number, char* data, int len)
{
    gwproxy::on_package(number, data, len);
}