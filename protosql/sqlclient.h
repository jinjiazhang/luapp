#ifndef _JINJIAZHANG_SQLCLIENT_H_
#define _JINJIAZHANG_SQLCLIENT_H_

#include "mysql.h"
#include "parambuf.h"
#include "resultbuf.h"
#include "google/protobuf/dynamic_message.h"

class sqlclient
{
public:
    sqlclient();
    ~sqlclient();

    bool connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

    typedef std::vector<std::shared_ptr<google::protobuf::Message>> result_set;
    int sql_select(const google::protobuf::Descriptor* descriptor, const std::string& condition, result_set& results);
    int sql_insert(const google::protobuf::Message* message);
    int sql_update(const google::protobuf::Message* message, const std::string& condition);
    int sql_delete(const google::protobuf::Descriptor* descriptor, const std::string& condition);
    int sql_create(const google::protobuf::Descriptor* descriptor);
    int sql_execute(const std::string& statement, std::vector<std::vector<std::string>>& table);

private:
    MYSQL* mysql_;
    parambuf parambuf_;
    resultbuf resultbuf_;
    google::protobuf::DynamicMessageFactory factory_;
};

#endif