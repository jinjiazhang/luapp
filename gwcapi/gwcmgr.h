#ifndef _JINJIAZHANG_GWCMGR_H_
#define _JINJIAZHANG_GWCMGR_H_

#include <map>
#include "gwcapi.h"

class gwconn;
class gwcmgr
{
private:
    gwcmgr();
    ~gwcmgr();

public:
    static gwcmgr* instance();
    int open(const char* url, gwctx* ctx);
    int close(int connid);
    int send(int connid, const void* data, int len);
    int sendv(int netid, iobuf bufs[], int count);
    int update(int timeout);

private:
    gwconn* get_conn(int connid);

private:
    typedef std::map<int, gwconn*> conn_map;
    conn_map conn_map_;
    int last_connid_;
};

#endif