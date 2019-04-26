#ifndef _JINJIAZHANG_SQLCLIENT_H_
#define _JINJIAZHANG_SQLCLIENT_H_

#include "mysql.h"
#include "parambuf.h"
#include "resultbuf.h"
#include "google/protobuf/message.h"

class sqlclient
{
public:
    sqlclient();
    ~sqlclient();

    bool connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

    int sql_select(const google::protobuf::Descriptor* descriptor, const std::string& condition);
    int sql_insert(const google::protobuf::Message* message);
    int sql_update(const google::protobuf::Message* message, const std::string& condition);
    int sql_delete(const google::protobuf::Descriptor* descriptor, const std::string& condition);

private:
    MYSQL* mysql_;
    parambuf parambuf_;
    resultbuf resultbuf_;
};

#endif