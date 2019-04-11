#ifndef _JINJIAZHANG_HTTP_H_
#define _JINJIAZHANG_HTTP_H_

#define CURL_STATICLIB

#include <string>
#include "curl/curl.h"

class http
{
public:
    struct callback
    {
        virtual void respond(int token, int code, const char* data) = 0;
    };

    struct taskdata
    {
        int token;
        callback* handle;
        std::string content;
    };

public:
    http();
    ~http();

public:
    int update();
    int get(callback* handle, const char* url);
    int post(callback* handle, const char* url, const char* data);

private:
    CURLM* multi_;
    CURLSH* share_;
    int last_token_;
};

#endif