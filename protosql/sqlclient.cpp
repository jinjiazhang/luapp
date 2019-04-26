#include "sqlclient.h"
#include "sqlutil.h"
#include "protolog/protolog.h"
#include <vector>

#include "google/protobuf/util/json_util.h"
using namespace google::protobuf;

sqlclient::sqlclient() : parambuf_(256*1024), resultbuf_(256*1024)
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

int sqlclient::sql_select(const Descriptor* descriptor, const std::string& condition)
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
        log_error("sqlclient::sql_select, result_parpare fail, ret: %d", ret);
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
    if (ret != 0 && ret != MYSQL_DATA_TRUNCATED)
    {
        log_error("sqlclient::sql_select, fetch fail, ret: %d", ret);
        return -1;
    }

    DynamicMessageFactory factory;
    const Message* prototype = factory.GetPrototype(descriptor);
    Message* message = prototype->New();
    ret = resultbuf_.parse(stmt, binds, message);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, fetch fail, ret: %d", ret);
        return -1;
    }

    int affected = (int)mysql_stmt_affected_rows(stmt);
    mysql_stmt_close(stmt);
    return affected;
}

int sqlclient::sql_insert(const Message* message)
{
    MYSQL_STMT* stmt = mysql_stmt_init(mysql_);
    std::string query = sqlutil::make_insert(message->GetDescriptor());
    int ret = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, parpare fail, ret: %d", ret);
        return -1;
    }

    std::vector<MYSQL_BIND> binds;
    ret = parambuf_.parpare(message, binds);
    if (ret != 0)
    {
        log_error("sqlclient::sql_insert, param_parpare fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_bind_param(stmt, binds.data());
    if (ret != 0)
    {
        log_error("sqlclient::sql_insert, bind_param fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_execute(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_insert, execute fail, ret: %d", ret);
        return -1;
    }

    int affected = (int)mysql_stmt_affected_rows(stmt);
    mysql_stmt_close(stmt);
    return affected;
}

int sqlclient::sql_update(const Message* message, const std::string& condition)
{
    MYSQL_STMT* stmt = mysql_stmt_init(mysql_);
    std::string query = sqlutil::make_update(message->GetDescriptor(), condition);
    int ret = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, parpare fail, ret: %d", ret);
        return -1;
    }

    std::vector<MYSQL_BIND> binds;
    ret = parambuf_.parpare(message, binds);
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, param_parpare fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_bind_param(stmt, binds.data());
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, bind_param fail, ret: %d", ret);
        return -1;
    }

    ret = mysql_stmt_execute(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, execute fail, ret: %d", ret);
        return -1;
    }

    int affected = (int)mysql_stmt_affected_rows(stmt);
    mysql_stmt_close(stmt);
    return affected;
}

int sqlclient::sql_delete(const Descriptor* descriptor, const std::string& condition)
{
    std::string query = sqlutil::make_delete(descriptor, condition);
    int ret = mysql_real_query(mysql_, query.c_str(), query.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_delete, parpare fail, ret: %d", ret);
        return -1;
    }

    int affected = (int)mysql_affected_rows(mysql_);
    return affected;
}