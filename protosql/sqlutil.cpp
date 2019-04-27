#include "sqlutil.h"
#include <sstream>

using namespace google::protobuf;

std::string sqlutil::make_select(const Descriptor* descriptor, const std::string& condition)
{
    std::stringstream stream;
    stream << "select ";
    int field_count = descriptor->field_count();
    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        stream << field->name();
        if (i + 1 < field_count) stream << ", ";
    }
    stream << " from " << descriptor->name();
    stream << " where " << condition;
    return stream.str();
}

std::string sqlutil::make_insert(const Descriptor* descriptor)
{
    std::stringstream stream;
    stream << "insert into " << descriptor->name() << "(";
    int field_count = descriptor->field_count();
    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        stream << field->name();
        if (i + 1 < field_count) stream << ", ";
    }
    stream << ") values(";
    for (int i = 0; i < field_count; i++)
    {
        stream << "?";
        if (i + 1 < field_count) stream << ", ";
    }
    stream << ")";
    return stream.str();
}

std::string sqlutil::make_update(const Descriptor* descriptor, const std::string& condition)
{
    std::stringstream stream;
    stream << "update " << descriptor->name() << " set ";
    int field_count = descriptor->field_count();
    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        stream << field->name() << " = ?";
        if (i + 1 < field_count) stream << ", ";
    }
    stream << " where " << condition;
    return stream.str();
}

std::string sqlutil::make_delete(const Descriptor* descriptor, const std::string& condition)
{
    std::stringstream stream;
    stream << "delete from " << descriptor->name();
    stream << " where " << condition;
    return stream.str();
}

std::string sqlutil::make_create(const google::protobuf::Descriptor* descriptor)
{
    std::stringstream stream;
    stream << "create table " << descriptor->name() << "(";
    int field_count = descriptor->field_count();
    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        stream << field->name() << " " << make_sql_type(field);
        if (i + 1 < field_count) stream << ", ";
    }
    stream << ")";
    return stream.str();
}

std::string sqlutil::make_sql_type(const google::protobuf::FieldDescriptor* field)
{
    if (field->is_map() || field->is_repeated())
    {
        return "blob";
    }

    switch (field->cpp_type())
    {
    case FieldDescriptor::CPPTYPE_DOUBLE:
        return "double";
    case FieldDescriptor::CPPTYPE_FLOAT:
        return "float";
    case FieldDescriptor::CPPTYPE_INT32:
    case FieldDescriptor::CPPTYPE_UINT32:
    case FieldDescriptor::CPPTYPE_ENUM:
        return "int";
    case FieldDescriptor::CPPTYPE_INT64:
    case FieldDescriptor::CPPTYPE_UINT64:
        return "bigint";
    case FieldDescriptor::CPPTYPE_BOOL:
        return "tinyint";
    case FieldDescriptor::CPPTYPE_STRING:
        return (field->type() == FieldDescriptor::TYPE_BYTES) ? "blob" : "varchar(255)";
    case FieldDescriptor::CPPTYPE_MESSAGE:
        return "blob";
    default:
        return "invalid";
    }
}