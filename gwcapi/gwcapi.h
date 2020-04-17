#ifndef _JINJIAZHANG_GWCAPI_H_
#define _JINJIAZHANG_GWCAPI_H_

typedef void (*gwcb_accept)(int connid, int error);
typedef void (*gwcb_closed)(int connid, int error);
typedef void (*gwcb_package)(int connid, char* data, int len);

int gwcapi_open(const char* url);
int gwcapi_close(int connid);
int gwcapi_send(int connid, const void* data, int len);
int gwcapi_on_accept(int connid, gwcb_accept callback);
int gwcapi_on_closed(int connid, gwcb_closed callback);
int gwcapi_on_package(int connid, gwcb_package callback);
int gwcapi_update(int timeout);

#endif