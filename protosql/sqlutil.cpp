#include "sqlutil.h"

std::string sqlutil::make_select(const google::protobuf::Descriptor* descriptor, const std::string& condition)
{
    return "select * from user";
}

std::string sqlutil::make_insert(const google::protobuf::Descriptor* descriptor, const std::string& condition)
{
    return std::string();
}

std::string sqlutil::make_update(const google::protobuf::Descriptor* descriptor, const std::string& condition)
{
    return std::string();
}

std::string sqlutil::make_delete(const google::protobuf::Descriptor* descriptor, const std::string& condition)
{
    return std::string();
}