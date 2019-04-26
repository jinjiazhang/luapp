#ifndef _JINJIAZHANG_SQLCLIENT_H_
#define _JINJIAZHANG_SQLCLIENT_H_

#include "mysql.h"
#include "resultbuf.h"
#include "google/protobuf/message.h"

class sqlclient
{
public:
    sqlclient();
    ~sqlclient();

    bool connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

    int sql_select(const google::protobuf::Descriptor* descriptor, const std::string& condition);
    int sql_insert(google::protobuf::Message* message, const std::string& condition);
    int sql_update(google::protobuf::Message* message, const std::string& condition);
    int sql_delete(const google::protobuf::Descriptor* descriptor, const std::string& condition);

private:
    MYSQL* mysql_;
    resultbuf resultbuf_;
};

#endif