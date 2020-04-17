#ifndef _JINJIAZHANG_GWCONN_H_
#define _JINJIAZHANG_GWCONN_H_

#include "gwtools.h"
#include "cipher.h"

class gwconn
{
public:
    gwconn(int connid);
    virtual ~gwconn();

    virtual bool init(url_info* args);
    virtual void on_accept(int connid, int error);
    virtual void on_closed(int connid, int error);
    virtual void on_package(int connid, char* data, int len);
    virtual void raw_package(int connid, char* data, int len);

    virtual void close();
    virtual void send(const void* data, int len);
    virtual void raw_send(const void* data, int len);
    virtual void recv_key(const char* key, int len);

protected:
    typedef std::vector<std::string> send_cache;

    cipher cipher_;
    send_cache send_cache_;

    int connid_;
    bool encrypt_;
    bool key_recv_;
};

#endif