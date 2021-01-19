#ifndef _JINJIAZHANG_GWCAPI_H_
#define _JINJIAZHANG_GWCAPI_H_

typedef void (*gwcb_accept)(int connid, int error);
typedef void (*gwcb_closed)(int connid, int error);
typedef void (*gwcb_package)(int connid, char* data, int len);

struct gwctx {
    gwcb_accept on_accept;
    gwcb_closed on_closed;
    gwcb_package on_package;
};

struct iobuf
{
    const void* data;
    int len;
};

int gwcapi_open(const char* url, gwctx* ctx);
int gwcapi_close(int connid);
int gwcapi_send(int connid, const void* data, int len);
int gwcapi_sendv(int connid, iobuf bufs[], int count);
int gwcapi_update(int timeout);

#endif