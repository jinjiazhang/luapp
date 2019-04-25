#ifndef _JINJIAZHANG_SQLCLIENT_H_
#define _JINJIAZHANG_SQLCLIENT_H_

#include "mysql.h"

class sqlclient
{
public:
    sqlclient();
    ~sqlclient();

    bool connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

private:
    MYSQL* mysql;
};

#endif