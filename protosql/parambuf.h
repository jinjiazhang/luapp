#ifndef _JINJIAZHANG_PARAMBUF_H_
#define _JINJIAZHANG_PARAMBUF_H_

#include "mysql.h"
#include "google/protobuf/message.h"
#include <vector>

class parambuf
{
public:
    parambuf(int length);
    ~parambuf();

    int parpare(const google::protobuf::Message* message, std::vector<MYSQL_BIND>& binds);

private:
    int parpare_field(const google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field, MYSQL_BIND& bind);
    int parpare_required(const google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field, MYSQL_BIND& bind);
    int parpare_repeated(const google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field, MYSQL_BIND& bind);
    
    template <typename T>
    int parpare_bind(MYSQL_BIND& bind, enum enum_field_types field_type, const T& value);

private:
    int length_;
    char* buffer_;
    char* current_;
};
#endif