#include "session.h"

session::session(network* instance, imanager* manager)
{
    network_ = instance;
    manager_ = manager;
}

session::~session()
{

}

bool session::init(socket_t fd)
{
    fd_ = fd;
    network_->add_event(this, fd_, EVENT_READ);
    return true;
}

void session::on_event(int events)
{
    if (events & EVENT_READ)
    {
        on_readable();
    }

    if (events & EVENT_WRITE)
    {
        on_writable();
    }
}

void session::on_error(int error)
{
    manager_->on_closed(number_, error);
    network_->close(number_);
}

void session::on_readable()
{
    while (true)
    {
        int recv_len = recv_data(fd_, recvbuf_.tail(), recvbuf_.space());
        if (recv_len < 0)
        {
            int error = get_socket_err();
            if (error != 0)
            {
                on_error(error);
                return;
            }
            break;
        }
        
        if (recv_len == 0)
        {
            on_error(0);
            return;
        }

        recvbuf_.pop_space(recv_len);
        this->dispatch();
    }
}

void session::on_writable()
{
    while (sendbuf_.size() > 0)
    {
        int send_len = send_data(fd_, sendbuf_.data(), sendbuf_.size());
        if (send_len < 0)
        {
            int error = get_socket_err();
            if (error != 0)
            {
                on_error(error);
                return;
            }
            break;
        }

        if (send_len == 0)
        {
            on_error(0);
            return;
        }

        sendbuf_.pop_data(send_len);
    }

    if (sendbuf_.size() <= 0)
    {
        sendbuf_.expand(0);
        network_->del_event(this, fd_, EVENT_WRITE);
    }
}

void session::send(char* data, int len)
{
    char head[16];
    int head_len = encode_var(head, sizeof(head), len);
    iovec iov[2];
    iov[0].iov_base = head;
    iov[0].iov_len = head_len;
    iov[1].iov_base = data;
    iov[1].iov_len = len;

    if (sendbuf_.size() > 0)
    {
        if (!sendbuf_.push_data(iov, 2, 0))
        {
            on_error(-1);
        }
        return;
    }

    int send_len = send_iovec(fd_, iov, 2);
    if (send_len < 0)
    {
        int error = get_socket_err();
        if (error != 0)
        {
            on_error(error);
            return;
        }

        if (!sendbuf_.push_data(iov, 2, 0))
        {
            on_error(-2);
            return;
        }
        network_->add_event(this, fd_, EVENT_WRITE);
        return;
    }

    if (send_len == 0)
    {
        on_error(0);
        return;
    }

    if (send_len < head_len + len)
    {
        if (!sendbuf_.push_data(iov, 2, send_len))
        {
            on_error(-3);
            return;
        }
        network_->add_event(this, fd_, EVENT_WRITE);
    }
}

void session::close()
{
    if (fd_ >= 0)
    {
        network_->del_event(this, fd_, events_);
        close_socket(fd_);
        fd_ = -1;
    }
}

void session::dispatch()
{
    while (recvbuf_.size() > 0)
    {
        int body_len = 0;
        int head_len = decode_var(&body_len, recvbuf_.data(), recvbuf_.size());
        if (head_len < 0)
        {
            on_error(-4);
            return;
        }

        if (head_len == 0)
        {
            break;
        }

        if (body_len <= 0)
        {
            on_error(-5);
            return;
        }

        if (!recvbuf_.expand(head_len + body_len))
        {
            on_error(-6);
            return;
        }

        if (recvbuf_.size() < head_len + body_len)
        {
            break;
        }

        recvbuf_.pop_data(head_len);
        manager_->on_package(number_, recvbuf_.data(), body_len);
        recvbuf_.pop_data(body_len);
    }

    recvbuf_.trim_data();
}