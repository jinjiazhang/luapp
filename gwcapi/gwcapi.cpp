#include "gwcapi.h"

int gwcapi_open(const char* url)
{
    return 0;
}

int gwcapi_close(int connid)
{
    return 0;
}

int gwcapi_send(int connid, const void* data, int len)
{
	return 0;
}

int gwcapi_on_accept(int connid, gwcb_accept callback)
{
    return 0;
}

int gwcapi_on_closed(int connid, gwcb_closed callback)
{
    return 0;
}

int gwcapi_on_package(int connid, gwcb_package callback)
{
    return 0;
}

int gwcapi_update(int timeout)
{
    return 0;
}