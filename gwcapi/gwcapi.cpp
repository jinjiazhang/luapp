#include "gwcapi.h"
#include "gwcmgr.h"

int gwcapi_open(const char* url, gwctx* ctx)
{
    return gwcmgr::instance()->open(url, ctx);
}

int gwcapi_close(int connid)
{
    return gwcmgr::instance()->close(connid);
}

int gwcapi_send(int connid, const void* data, int len)
{
    return gwcmgr::instance()->send(connid, data, len);
}

int gwcapi_update(int timeout)
{
    return gwcmgr::instance()->update(timeout);
}