#include "sqlclient.h"
#include "sqlutil.h"
#include "protolog/protolog.h"
#include <vector>

using namespace google::protobuf;
#define mysql_code(ret) (((ret) > 0) ? -(ret) : (ret))

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

int sqlclient::sql_select(const Descriptor* descriptor, const std::string& condition, result_set& results)
{
    MYSQL_STMT* stmt = mysql_stmt_init(mysql_);
    std::string query = sqlutil::make_select(descriptor, condition);
    int ret = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, parpare fail, ret: %d", ret);
        return mysql_code(ret);
    }

    std::vector<MYSQL_BIND> binds;
    ret = resultbuf_.parpare(descriptor, binds);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, result_parpare fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_bind_result(stmt, binds.data());
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, bind_result fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_execute(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, execute fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_store_result(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_select, store_result fail, ret: %d", ret);
        return mysql_code(ret);
    }

    int count = (int)mysql_stmt_num_rows(stmt);
    results.resize(count);
    for (int i = 0; i < count; i++)
    {
        ret = mysql_stmt_fetch(stmt);
        if (ret != 0 && ret != MYSQL_DATA_TRUNCATED)
        {
            log_error("sqlclient::sql_select, fetch fail, ret: %d", ret);
            return mysql_code(ret);
        }
        const Message* prototype = factory_.GetPrototype(descriptor);
        std::shared_ptr<Message> message(prototype->New());
        ret = resultbuf_.parse(stmt, binds, message.get());
        if (ret != 0)
        {
            log_error("sqlclient::sql_select, fetch fail, ret: %d", ret);
            return mysql_code(ret);
        }
        results[i] = message;
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
        return mysql_code(ret);
    }

    std::vector<MYSQL_BIND> binds;
    ret = parambuf_.parpare(message, binds);
    if (ret != 0)
    {
        log_error("sqlclient::sql_insert, param_parpare fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_bind_param(stmt, binds.data());
    if (ret != 0)
    {
        log_error("sqlclient::sql_insert, bind_param fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_execute(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_insert, execute fail, ret: %d", ret);
        return mysql_code(ret);
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
        return mysql_code(ret);
    }

    std::vector<MYSQL_BIND> binds;
    ret = parambuf_.parpare(message, binds);
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, param_parpare fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_bind_param(stmt, binds.data());
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, bind_param fail, ret: %d", ret);
        return mysql_code(ret);
    }

    ret = mysql_stmt_execute(stmt);
    if (ret != 0)
    {
        log_error("sqlclient::sql_update, execute fail, ret: %d", ret);
        return mysql_code(ret);
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
        log_error("sqlclient::sql_delete, real_query fail, ret: %d", ret);
        return mysql_code(ret);
    }

    int affected = (int)mysql_affected_rows(mysql_);
    return affected;
}

int sqlclient::sql_create(const google::protobuf::Descriptor* descriptor)
{
    std::string query = sqlutil::make_create(descriptor);
    int ret = mysql_real_query(mysql_, query.c_str(), query.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_create, real_query fail, ret: %d", ret);
        return mysql_code(ret);
    }

    int affected = (int)mysql_affected_rows(mysql_);
    return affected;
}

int sqlclient::sql_execute(const std::string& statement, std::vector<std::vector<std::string>>& table)
{
    int ret = mysql_real_query(mysql_, statement.c_str(), statement.size());
    if (ret != 0)
    {
        log_error("sqlclient::sql_execute, real_query fail, ret: %d", ret);
        return mysql_code(ret);
    }

    MYSQL_RES* result = mysql_store_result(mysql_);
    if (result != nullptr)
    {
        int row_count = (int)mysql_num_rows(result);
        int field_count = (int)mysql_num_fields(result);
        if (row_count > 0 && field_count > 0)
        {
            MYSQL_FIELD* fields = mysql_fetch_field(result);
            if (fields == nullptr)
            {
                log_error("sqlclient::sql_execute, fields nullptr, ret: %d", ret);
                return -1;
            }

            table.resize(row_count + 1);
            table[0].resize(field_count);
            for (int i = 0; i < field_count; i++)
            {
                table[0][i] = fields[i].name;
            }

            for (int j = 0; j < row_count; j++)
            {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row == nullptr)
                {
                    log_error("sqlclient::sql_execute, rows nullptr, ret: %d", ret);
                    return -1;
                }

                table[j + 1].resize(field_count);
                for (int i = 0; i < field_count; i++)
                {
                    table[j + 1][i] = row[i];
                }
            }
        }
    }

    int affected = (int)mysql_affected_rows(mysql_);
    return affected;
}