#include "session.h"
#include "varint.h"
#include <vector>

session::session(network* instance, imanager* manager)
{
    fd_ = -1;
    network_ = instance;
    manager_ = manager;
    closed_ = true;
}

session::~session()
{

}

bool session::init(socket_t fd)
{
    fd_ = fd;
    closed_ = false;
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
    while (!closed_)
    {
        int recv_len = recv_data(fd_, recvbuf_.tail(), recvbuf_.space());
        if (recv_len < 0)
        {
            int error = get_network_err();
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
    while (!closed_ && sendbuf_.size() > 0)
    {
        int send_len = send_data(fd_, sendbuf_.data(), sendbuf_.size());
        if (send_len < 0)
        {
            int error = get_network_err();
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

void session::send(const void* data, int len)
{
    char head_data[16];
    int head_len = encode_varint(head_data, sizeof(head_data), len);
    iovec iov[2];
    iov[0].iov_base = head_data;
    iov[0].iov_len = head_len;
    iov[1].iov_base = (char*)data;
    iov[1].iov_len = len;
    transmit(iov, 2);
}

void session::sendv(iobuf bufs[], int count)
{
    int total = 0;
    std::vector<iovec> iov(count + 1);
    for (int i = 0; i < count; i++)
    {
        iov[i+1].iov_base = (char*)bufs[i].data;
        iov[i+1].iov_len = bufs[i].len;
        total += bufs[i].len;
    }

    char head_data[16];
    int head_len = encode_varint(head_data, sizeof(head_data), total);
    iov[0].iov_base = head_data;
    iov[0].iov_len = head_len;
    transmit(iov.data(), iov.size());
}

void session::transmit(iovec* iov, int iovcnt)
{
    if (closed_ || sendbuf_.size() > 0)
    {
        if (!sendbuf_.push_data(iov, iovcnt, 0))
        {
            on_error(-1);
        }
        return;
    }

    int send_len = send_iovec(fd_, iov, iovcnt);
    if (send_len < 0)
    {
        int error = get_network_err();
        if (error != 0)
        {
            on_error(error);
            return;
        }

        if (!sendbuf_.push_data(iov, iovcnt, 0))
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

    int total = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        total += iov[i].iov_len;
    }

    if (send_len < total)
    {
        if (!sendbuf_.push_data(iov, iovcnt, send_len))
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

    closed_ = true;
}

void session::dispatch()
{
    while (!closed_ && recvbuf_.size() > 0)
    {
        int body_len = 0;
        int head_len = decode_varint(&body_len, recvbuf_.data(), recvbuf_.size());
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