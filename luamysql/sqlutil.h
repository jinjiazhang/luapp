#ifndef _JINJIAZHANG_SQLUTIL_H_
#define _JINJIAZHANG_SQLUTIL_H_

#include <string>
#include "google/protobuf/dynamic_message.h"

class sqlutil {
public:
    static std::string make_select(const google::protobuf::Descriptor* descriptor, const std::string& condition);
    static std::string make_insert(const google::protobuf::Descriptor* descriptor);
    static std::string make_update(const google::protobuf::Descriptor* descriptor, const std::string& condition);
    static std::string make_delete(const google::protobuf::Descriptor* descriptor, const std::string& condition);
    static std::string make_create(const google::protobuf::Descriptor* descriptor, const std::vector<std::string>& primarys);
    static std::string make_sql_type(const google::protobuf::FieldDescriptor* field);
};

#endif