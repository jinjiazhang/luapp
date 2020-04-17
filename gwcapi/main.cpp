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
    int connid = gwcapi_open("tcp://127.0.0.1:6575?encrypt=1");
    gwcapi_on_accept(connid, on_accept);
    gwcapi_on_closed(connid, on_closed);
    gwcapi_on_package(connid, on_package);
    gwcapi_update(0);
}