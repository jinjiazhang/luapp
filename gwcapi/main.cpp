#include "gwcapi.h"
#include <stdio.h>

void on_accept(int connid, int error)
{
    printf("on_accept connid=%d, error=%d", connid, error);
}

void on_closed(int connid, int error)
{
    printf("on_closed connid=%d, error=%d", connid, error);
}

void on_package(int connid, char* data, int len)
{
    printf("on_package connid=%d, len=%d, data=%s", connid, len, data);
}

int main()
{
    gwctx ctx;
    ctx.on_accept = on_accept;
    ctx.on_closed = on_closed;
    ctx.on_package = on_package;
    const char* url = "tcp://127.0.0.1:6575?encrypt=1";
    gwcapi_open(url, &ctx);
    gwcapi_update(0);
}