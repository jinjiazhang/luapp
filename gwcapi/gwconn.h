#ifndef _JINJIAZHANG_GWCONN_H_
#define _JINJIAZHANG_GWCONN_H_

#include "gwcapi.h"
#include "gwtools.h"
#include "cipher.h"

class gwconn
{
public:
    gwconn(int connid);
    ~gwconn();

    bool init(url_info* args, gwctx* ctx);
    void on_accept(int connid, int error);
    void on_closed(int connid, int error);
    void on_package(int connid, char* data, int len);
    void raw_package(int connid, char* data, int len);

    void close();
    void send(const void* data, int len);
    void raw_send(const void* data, int len);
    void recv_key(const char* key, int len);

protected:
    typedef std::vector<std::string> send_cache;

    cipher cipher_;
    send_cache send_cache_;

    int connid_;
    bool encrypt_;
    bool key_recv_;
};

#endif