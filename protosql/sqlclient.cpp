#include "sqlclient.h"
#include "sqlutil.h"
#include "protolog/protolog.h"
#include "protolog/protolog.h"
#include <vector>

sqlclient::sqlclient() : resultbuf_(256*1024)
{
    mysql_ = mysql_init(NULL);
}

sqlclient::~sqlclient()
{
    if (mysql_)
        mysql_close(mysql_);
}

bool sqlclient::connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
    unsigned int timeout = 3000;
    mysql_options(mysql_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    if (mysql_real_connect(mysql_, host, user, passwd, db, port, NULL, 0) == NULL)
    {
        return false;
    }
    return true;
}

int sqlclient::sql_select(const google::protobuf::Descriptor* descriptor, const std::string& condition)
{
    MYSQL_STMT* stmt = mysql_stmt_init(mysql_);
    std::string query = sqlutil::make_select(descriptor, condition);
    int ret = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, parpare fail, ret: %d", ret);
        return -1;
    }

    std::vector<MYSQL_BIND> binds;
    ret = resultbuf_.parpare(descriptor, binds);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, bind_parpare fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_bind_result(stmt, binds.data());
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, bind_result fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_execute(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, execute fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_store_result(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, store_result fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_fetch(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, fetch fail, ret: %d", ret);
        return -1;
    }

    google::protobuf::DynamicMessageFactory factory;
    const google::protobuf::Message* prototype = factory.GetPrototype(descriptor);
    google::protobuf::Message* message = prototype->New();
    ret = resultbuf_.parse(stmt, binds, message);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, fetch fail, ret: %d", ret);
        return -1;
    }
    return 0;
}

int sqlclient::sql_insert(google::protobuf::Message* message, const std::string& condition)
{
    return 0;
}

int sqlclient::sql_update(google::protobuf::Message* message, const std::string& condition)
{
    return 0;
}

int sqlclient::sql_delete(const google::protobuf::Descriptor* descriptor, const std::string& condition)
{
    return 0;
}