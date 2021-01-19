#ifndef _JINJIAZHANG_GWCMGR_H_
#define _JINJIAZHANG_GWCMGR_H_

#include "gwcapi.h"

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
};

#endif