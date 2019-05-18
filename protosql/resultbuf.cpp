#include "resultbuf.h"
#include "protolog/protolog.h"
#include <google/protobuf/wire_format.h>
#include <google/protobuf/wire_format_lite.h>
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::internal;

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
    bind.length = &bind.length_value;
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
        if (field->type() == FieldDescriptor::TYPE_BYTES)
            parpare_bind(bind, MYSQL_TYPE_BLOB, BLOB_PARPARE_LENGTH);
        else
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
        ret = fetch_buffer(stmt, bind[i], i);
        if (ret != 0)
        {
            log_error("resultbuf::parse fetch_buffer fail, name=%s", field->full_name().c_str());
            break;
        }

        ret = parse_field(stmt, bind[i], message, field);
        if (ret != 0)
        {
            log_error("resultbuf::parse parse_field fail, name=%s", field->full_name().c_str());
            break;
        }
    }
    
    return ret;
}

int resultbuf::fetch_buffer(MYSQL_STMT* stmt, MYSQL_BIND& bind, unsigned int column)
{
    if (bind.length_value <= bind.buffer_length)
    {
        return 0;
    }

    if (bind.length_value > templen_)
    {
        if (tempbuf_)
            delete[] tempbuf_;
        tempbuf_ = new char[bind.length_value + 1];
        templen_ = bind.length_value + 1;
    }

    bind.buffer = tempbuf_;
    bind.buffer_length = templen_;
    int ret = mysql_stmt_fetch_column(stmt, &bind, column, 0);
    if (ret != 0)
    {
        return -1;
    }
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
        reflection->SetString(message, field, std::string((char*)bind.buffer, *bind.length));
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
        submessage = reflection->MutableMessage(message, field);
        submessage->ParseFromArray(bind.buffer, *bind.length);
        break;
    default:
        log_error("resultbuf::parse_required field unknow type, field=%s", field->full_name().c_str());
        return false;
    }
    return 0;
}

// TODO: check return value
#define DECODE_SINGLE_FIELD(CType, WType, input, AddFunc) \
    do { \
        CType value; \
        WireFormatLite::ReadPrimitive<CType, WType>(input, &value); \
        reflection->AddFunc(message, field, value); \
    } while (0)

int decode_single(CodedInputStream* input, Message* message, const FieldDescriptor* field)
{
    const Reflection* reflection = message->GetReflection();
    switch (field->type())
    {
    case FieldDescriptor::TYPE_DOUBLE:
        DECODE_SINGLE_FIELD(double, WireFormatLite::TYPE_DOUBLE, input, AddDouble);
        break;
    case FieldDescriptor::TYPE_FLOAT:
        DECODE_SINGLE_FIELD(float, WireFormatLite::TYPE_FLOAT, input, AddFloat);
        break;
    case FieldDescriptor::TYPE_INT64:
        DECODE_SINGLE_FIELD(int64, WireFormatLite::TYPE_INT64, input, AddInt64);
        break;
    case FieldDescriptor::TYPE_UINT64:
        DECODE_SINGLE_FIELD(uint64, WireFormatLite::TYPE_UINT64, input, AddUInt64);
        break;
    case FieldDescriptor::TYPE_INT32:
        DECODE_SINGLE_FIELD(int32, WireFormatLite::TYPE_INT32, input, AddInt32);
        break;
    case FieldDescriptor::TYPE_UINT32:
        DECODE_SINGLE_FIELD(uint32, WireFormatLite::TYPE_UINT32, input, AddUInt32);
        break;
    case FieldDescriptor::TYPE_FIXED64:
        DECODE_SINGLE_FIELD(uint64, WireFormatLite::TYPE_FIXED64, input, AddUInt64);
        break;
    case FieldDescriptor::TYPE_FIXED32:
        DECODE_SINGLE_FIELD(uint32, WireFormatLite::TYPE_FIXED32, input, AddUInt32);
        break;
    case FieldDescriptor::TYPE_SFIXED32:
        DECODE_SINGLE_FIELD(int32, WireFormatLite::TYPE_SFIXED32, input, AddInt32);
        break;
    case FieldDescriptor::TYPE_SFIXED64:
        DECODE_SINGLE_FIELD(int64, WireFormatLite::TYPE_SFIXED64, input, AddInt64);
        break;
    case FieldDescriptor::TYPE_SINT32:
        DECODE_SINGLE_FIELD(int32, WireFormatLite::TYPE_SINT32, input, AddInt32);
        break;
    case FieldDescriptor::TYPE_SINT64:
        DECODE_SINGLE_FIELD(int64, WireFormatLite::TYPE_SINT64, input, AddInt64);
        break;
    case FieldDescriptor::TYPE_ENUM:
        DECODE_SINGLE_FIELD(int, WireFormatLite::TYPE_ENUM, input, AddEnumValue);
        break;
    case FieldDescriptor::TYPE_BOOL:
        DECODE_SINGLE_FIELD(bool, WireFormatLite::TYPE_BOOL, input, AddBool);
        break;
    case FieldDescriptor::TYPE_STRING:
    case FieldDescriptor::TYPE_BYTES:
        {
            std::string value;
            input->ReadTagNoLastTag();
            WireFormatLite::ReadString(input, &value);
            reflection->AddString(message, field, value);
        }
        break;
    case FieldDescriptor::TYPE_MESSAGE:
        {
            input->ReadTagNoLastTag();
            Message* submessage = reflection->AddMessage(message, field);
            WireFormatLite::ReadMessage(input, submessage);
        }
        break;
    default:
        return -1;
    }
    return 0;
}

int resultbuf::parse_repeated(MYSQL_STMT* stmt, MYSQL_BIND& bind, Message* message, const FieldDescriptor* field)
{
    ArrayInputStream buffer(bind.buffer, bind.length_value);
    CodedInputStream stream(&buffer);

    CodedInputStream::Limit limit = stream.PushLimit(bind.length_value);
    while (stream.BytesUntilLimit() > 0)
    {
        int ret = decode_single(&stream, message, field);
        if (ret != 0)
        {
            log_error("resultbuf::parse_repeated decode field fail, field=%s", field->full_name().c_str());
            return ret;
        }
    }
    stream.PopLimit(limit);
    return 0;
}