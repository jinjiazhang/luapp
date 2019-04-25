#include "sqlclient.h"

sqlclient::sqlclient()
{
    mysql = mysql_init(NULL);
}

sqlclient::~sqlclient()
{
    if (mysql)
        mysql_close(mysql);
}

bool sqlclient::connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
    unsigned int timeout = 3000;
    mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) == NULL)
    {
        return false;
    }
    return true;
}