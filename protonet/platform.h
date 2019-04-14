#ifndef _JINJIAZHANG_PLATFORM_H_
#define _JINJIAZHANG_PLATFORM_H_

#include <map>
#include <string.h>
#include <algorithm>
#include <assert.h>

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
typedef int socket_t;
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
typedef int socket_t;
struct iovec{
    unsigned long iov_len; /* Length of data. */
    char *iov_base; /* Pointer to data. */
};
#endif

socket_t net_listen(const char* ip, int port);
socket_t net_connect(const char* ip, int port);
socket_t net_accept(socket_t listenfd, sockaddr_in& addr);
int recv_data(socket_t fd, char* data, int len);
int send_data(socket_t fd, char* data, int len);
int send_iovec(socket_t fd, iovec *iov, int cnt);
int close_socket(socket_t fd);
int set_no_block(socket_t fd);
int set_no_delay(socket_t fd);
int set_reuse_addr(socket_t fd);
int get_socket_err(socket_t fd);
int get_network_err();

int encode_var(char* data, int len, int var);
int decode_var(int* var, char* data, int len);

#endif
