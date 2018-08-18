#include "connector.h"

connector::connector(network* instance, imanager* manager)
:session(instance, manager)
{
    connectfd_ = -1;
    connected_ = false;
}

connector::~connector()
{

}

bool connector::connect(const char* ip, int port)
{
    connectfd_ = net_connect(ip, port);
    if (connectfd_ < 0)
    {
        return false;
    }

    network_->add_event(this, connectfd_, EVENT_WRITE);
    return true;
}

void connector::on_event(int events, int param)
{
    if (connected_)
    {
        session::on_event(events, param);
        return;
    }

    if (events & EVENT_ERROR)
    {
        manager_->on_accept(number_, param);
        return;
    }

    assert(events & EVENT_WRITE);
    int error = get_socket_err(connectfd_);
    if (error != 0)
    {
        on_error(error);
        return;
    }

    network_->del_event(this, connectfd_, EVENT_WRITE);
    if (!this->init(connectfd_))
    {
        on_error(-1);
        return;
    }

    connected_ = true;
    manager_->on_accept(number_, 0);
}

void connector::on_error(int error)
{
    network_->mark_error(this, error);
}

void connector::send(char* data, int len)
{
    if (connected_)
    {
        session::send(data, len);
    }
}

void connector::close()
{
    if (connected_)
    {
        session::close();
    }
    else
    {
        if (connectfd_ >= 0)
        {
            network_->del_event(this, connectfd_, events_);
            close_socket(connectfd_);
            connectfd_ = -1;
            connected_ = false;
        }
    }
}