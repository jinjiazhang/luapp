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
    int parpare_bind(MYSQL_BIND& bind, enum enum_field_types field_type, unsigned long  buffer_length);
    int parpare_field(const google::protobuf::FieldDescriptor* field, MYSQL_BIND& bind);
    int parse_field(MYSQL_STMT* stmt, MYSQL_BIND& bind, google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field);
    int parse_required(MYSQL_STMT* stmt, MYSQL_BIND& bind, google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field);
    int parse_repeated(MYSQL_STMT* stmt, MYSQL_BIND& bind, google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field);
    int fetch_buffer(MYSQL_STMT* stmt, MYSQL_BIND& bind);

private:
    int length_;
    char* buffer_;
    char* current_;
    
    int templen_;
    char* tempbuf_;
};

#endif