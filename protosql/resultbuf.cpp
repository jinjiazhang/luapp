#include "resultbuf.h"
#include "protolog/protolog.h"

using namespace google::protobuf;

#define BLOB_PARPARE_LENGTH 2048
#define STRING_PARPARE_LENGTH 256

resultbuf::resultbuf(int length)
{
    length_ = length;
    buffer_ = new char[length];
    current_ = buffer_;
    templen_ = 0;
    tempbuf_ = nullptr;
}

resultbuf::~resultbuf()
{
    if (buffer_)
    {
        delete[] buffer_;
        buffer_ = nullptr;
        current_ = nullptr;
        length_ = 0;
    }

    if (tempbuf_)
    {
        delete[] tempbuf_;
        tempbuf_ = nullptr;
        templen_ = 0;
    }
}

int resultbuf::parpare(const Descriptor* descriptor, std::vector<MYSQL_BIND>& binds)
{
    int ret = 0;
    int field_count = descriptor->field_count();
    binds.resize(field_count);
    current_ = buffer_;

    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        ret = parpare_field(field, binds[i]);
        if (ret != 0 || current_ > buffer_ + length_)
        {
            log_error("resultbuf::parpare parpare_field fail, name=%s", field->full_name().c_str());
            break;
        }
    }

    return ret;
}

int resultbuf::parpare_bind(MYSQL_BIND& bind, enum enum_field_types field_type, unsigned long  buffer_length)
{
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = field_type;
    bind.buffer = current_;
    bind.buffer_length = buffer_length;
    bind.length = &bind.buffer_length;
    bind.is_null = &bind.is_null_value;
    current_ += buffer_length;
    return 0;
}

int resultbuf::parpare_field(const FieldDescriptor* field, MYSQL_BIND& bind)
{
    if (field->is_map() || field->is_repeated())
    {
        parpare_bind(bind, MYSQL_TYPE_BLOB, BLOB_PARPARE_LENGTH);
        return 0;
    }

    switch (field->cpp_type())
    {
    case FieldDescriptor::CPPTYPE_DOUBLE:
        parpare_bind(bind, MYSQL_TYPE_DOUBLE, sizeof(double));
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        parpare_bind(bind, MYSQL_TYPE_FLOAT, sizeof(float));
        break;
    case FieldDescriptor::CPPTYPE_INT32:
    case FieldDescriptor::CPPTYPE_UINT32:
    case FieldDescriptor::CPPTYPE_ENUM:
        parpare_bind(bind, MYSQL_TYPE_LONG, sizeof(int32));
        break;
    case FieldDescriptor::CPPTYPE_INT64:
    case FieldDescriptor::CPPTYPE_UINT64:
        parpare_bind(bind, MYSQL_TYPE_LONGLONG, sizeof(int64));
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        parpare_bind(bind, MYSQL_TYPE_TINY, sizeof(uint8));
        break;
    case FieldDescriptor::CPPTYPE_STRING:
        parpare_bind(bind, MYSQL_TYPE_VAR_STRING, STRING_PARPARE_LENGTH);
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
        parpare_bind(bind, MYSQL_TYPE_BLOB, BLOB_PARPARE_LENGTH);
        break;
    default:
        return -1;
    }
    return 0;
}

int resultbuf::parse(MYSQL_STMT* stmt, std::vector<MYSQL_BIND>& bind, Message* message)
{
    int ret = 0;
    const Descriptor* descriptor = message->GetDescriptor();
    int field_count = descriptor->field_count();
    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        ret = parse_field(stmt, bind[i], message, field);
        if (ret != 0 || current_ > buffer_ + length_)
        {
            log_error("resultbuf::parse parse_field fail, name=%s", field->full_name().c_str());
            break;
        }
    }
    
    return ret;
}

int resultbuf::fetch_buffer(MYSQL_STMT* stmt, MYSQL_BIND& bind)
{
    return 0;
}

int resultbuf::parse_field(MYSQL_STMT* stmt, MYSQL_BIND& bind, Message* message, const FieldDescriptor* field)
{
    if (field->is_map() || field->is_repeated())
    {
        return parse_repeated(stmt, bind, message, field);
    }
    else
    {
        return parse_required(stmt, bind, message, field);
    }
    return 0;
}

int resultbuf::parse_required(MYSQL_STMT* stmt, MYSQL_BIND& bind, Message* message, const FieldDescriptor* field)
{
    Message* submessage = nullptr;
    const Reflection* reflection = message->GetReflection();
    switch (field->cpp_type())
    {
    case FieldDescriptor::CPPTYPE_DOUBLE:
        reflection->SetDouble(message, field, *(double*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        reflection->SetFloat(message, field, *(float*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_INT32:
        reflection->SetInt32(message, field, *(int32*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_UINT32:
        reflection->SetUInt32(message, field, *(uint32*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_INT64:
        reflection->SetInt64(message, field, *(uint64*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_UINT64:
        reflection->SetUInt64(message, field, *(uint64*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_ENUM:
        reflection->SetEnumValue(message, field, *(int32*)bind.buffer);
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        reflection->SetBool(message, field, *(uint8*)bind.buffer != 0);
        break;
    case FieldDescriptor::CPPTYPE_STRING:
        fetch_buffer(stmt, bind);
        reflection->SetString(message, field, std::string((char*)bind.buffer, *bind.length));
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
        fetch_buffer(stmt, bind);
        submessage = reflection->MutableMessage(message, field);
        submessage->ParseFromArray(bind.buffer, *bind.length);
        break;
    default:
        log_error("resultbuf::parse_required field unknow type, field=%s", field->full_name().c_str());
        return false;
    }
    return 0;
}

int resultbuf::parse_repeated(MYSQL_STMT* stmt, MYSQL_BIND& bind, Message* message, const FieldDescriptor* field)
{
    return -1;
}