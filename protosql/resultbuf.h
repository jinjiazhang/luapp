#ifndef _JINJIAZHANG_RESULTBUF_H_
#define _JINJIAZHANG_RESULTBUF_H_

#include "mysql.h"
#include "google/protobuf/message.h"
#include <vector>

class resultbuf
{
public:
    resultbuf(int length);
    ~resultbuf();

    int parpare(const google::protobuf::Descriptor* descriptor, std::vector<MYSQL_BIND>& binds);
    int parse(MYSQL_STMT* stmt, std::vector<MYSQL_BIND>& bind, google::protobuf::Message* message);

private:
    int parpare_field(const google::protobuf::FieldDescriptor* field, MYSQL_BIND& bind);

private:
    int length_;
    char* buffer_;
    char* current_;
};

#endif