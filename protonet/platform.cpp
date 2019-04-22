#include "platform.h"

socket_t net_listen(const char* ip, int port)
{
    sockaddr_in addr;
    int len = sizeof (struct sockaddr_in);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    
    socket_t fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) 
    {
        return -1;
    }

    set_no_block(fd);
    set_reuse_addr(fd);

    if (::bind(fd, (struct sockaddr*)&addr, len) < 0) 
    {
        close_socket(fd);
        return -1;
    }

    if (::listen(fd, 5) < 0) 
    {
        close_socket(fd);
        return -1;
    }

    return fd;
}

socket_t net_connect(const char* ip, int port)
{
    sockaddr_in addr;
    int len = sizeof (struct sockaddr_in);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    socket_t fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) 
    {
        return -1;
    }

    set_no_block(fd);
    set_no_delay(fd);

    if (::connect(fd, (struct sockaddr*)&addr, len) < 0)
    {
        if (get_network_err() != 0)
        {
            close_socket(fd);
            return -1;
        }
    }

    return fd;
}

socket_t net_accept(socket_t listenfd, sockaddr_in& addr)
{
#ifdef __linux__
    unsigned int len = sizeof (struct sockaddr_in);
#else
    int len = sizeof (struct sockaddr_in);
#endif

    socket_t fd = ::accept(listenfd, (struct sockaddr*)&addr, &len);
    if (fd < 0)
    {
        return -1;
    }

    set_no_block(fd);
    set_no_delay(fd);
    return fd;
}

int recv_data(socket_t fd, char* data, int len)
{
    return ::recv(fd, data, len, 0);
}

int send_data(socket_t fd, char* data, int len)
{
    return ::send(fd, data, len, 0);
}

int send_iovec(socket_t fd, iovec *iov, int cnt)
{
#ifdef __linux__
    return ::writev(fd, iov, cnt);
#else
    unsigned long sendLen = 0;
    int ret = WSASend(fd, (WSABUF*)iov, cnt, &sendLen, 0, NULL, NULL);
    if (ret != 0)
    {
        return ret;
    }
    return sendLen;
#endif
}

int close_socket(socket_t fd)
{
#ifdef __linux__
    return ::close(fd);
#else
    return ::closesocket(fd);
#endif
}

int set_no_block(socket_t fd)
{
#ifdef __linux__
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    u_long argp = 1;
    return ::ioctlsocket(fd, FIONBIO, &argp);
#endif
}

int set_no_delay(socket_t fd)
{
    int enable = 1;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
}

int set_reuse_addr(socket_t fd)
{
    int enable = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
}

int get_socket_err(socket_t fd)
{
    int error = 0;
    int errlen = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error, (socklen_t*)&errlen) != 0)
    {
        return -1;
    }
    return error;
}

int get_network_err()
{
#ifdef __linux__
    int error = errno;
    if (error == 0 || error == EINTR || error == EAGAIN || error == EINPROGRESS)
    {
        return 0;
    }
#else
    int error = WSAGetLastError();
    if (error == 0 || error == WSAEINTR || error == WSAEWOULDBLOCK || error == WSAEINPROGRESS)
    {
        return 0;
    }
#endif
    return error;
}
