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

void connector::on_event(int events)
{
    if (connected_)
    {
        session::on_event(events);
        return;
    }

    assert(events & EVENT_WRITE);
    int error = get_socket_err(connectfd_);
    if (error != 0)
    {
        on_error(error);
        return;
    }

    if (!session::init(connectfd_))
    {
        on_error(NET_INNER_ERROR);
        return;
    }

    connected_ = true;
    manager_->on_accept(number_, 0);
    session::on_event(events);
}

void connector::on_error(int error)
{
    manager_->on_accept(number_, error);
    network_->close(number_);
}

void connector::close()
{
    if (connected_)
    {
        session::close();
        return;
    }

    if (connectfd_ >= 0)
    {
        network_->del_event(this, connectfd_, events_);
        close_socket(connectfd_);
        connectfd_ = -1;
        connected_ = false;
    }
}