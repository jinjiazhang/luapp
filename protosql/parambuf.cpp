#include "parambuf.h"
#include "protolog/protolog.h"
#include <google/protobuf/wire_format.h>
#include <google/protobuf/wire_format_lite.h>
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::internal;

parambuf::parambuf(int length)
{
    length_ = length;
    buffer_ = new char[length];
    current_ = buffer_;
}

parambuf::~parambuf()
{
    if (buffer_)
    {
        delete[] buffer_;
        buffer_ = nullptr;
        current_ = nullptr;
        length_ = 0;
    }
}

int parambuf::parpare(const Message* message, std::vector<MYSQL_BIND>& binds)
{
    int ret = 0;
    const Descriptor* descriptor = message->GetDescriptor();
    int field_count = descriptor->field_count();
    binds.resize(field_count);
    current_ = buffer_;

    for (int i = 0; i < field_count; i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        ret = parpare_field(message, field, binds[i]);
        if (ret != 0 || current_ > buffer_ + length_)
        {
            log_error("resultbuf::parpare parpare_field fail, name=%s", field->full_name().c_str());
            return ret;
        }
    }

    return ret;
}

int parambuf::parpare_field(const Message* message, const FieldDescriptor* field, MYSQL_BIND& bind)
{
    if (field->is_map() || field->is_repeated())
    {
        return parpare_repeated(message, field, bind);
    }
    else
    {
        return parpare_required(message, field, bind);
    }
    return 0;
}

template <typename T>
int parambuf::parpare_bind(MYSQL_BIND& bind, enum enum_field_types field_type, const T& value)
{
    if (current_ + sizeof(value) > buffer_ + length_)
    {
        log_error("parambuf::parpare_bind buffer not enought, size=%d", sizeof(value));
        return -1;
    }

    *(T*)current_ = value;
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = field_type;
    bind.buffer = current_;
    bind.buffer_length = sizeof(value);
    bind.length = &bind.length_value;
    bind.is_null = &bind.is_null_value;
    current_ += sizeof(value);
    return 0;
}

template <>
int parambuf::parpare_bind(MYSQL_BIND& bind, enum enum_field_types field_type, const std::string& value)
{
    if (current_ + value.size() > buffer_ + length_)
    {
        log_error("parambuf::parpare_bind buffer not enought, size=%d", value.size());
        return -1;
    }

    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = field_type;
    bind.buffer = current_;
    memcpy(current_, value.data(), value.size());
    bind.buffer_length = value.size();
    current_ += value.size();
    return 0;
}

int parambuf::parpare_required(const Message* message, const FieldDescriptor* field, MYSQL_BIND& bind)
{
    const Reflection* reflection = message->GetReflection();
    switch (field->cpp_type())
    {
    case FieldDescriptor::CPPTYPE_DOUBLE:
        return parpare_bind(bind, MYSQL_TYPE_DOUBLE, reflection->GetDouble(*message, field));
    case FieldDescriptor::CPPTYPE_FLOAT:
        return parpare_bind(bind, MYSQL_TYPE_FLOAT, reflection->GetFloat(*message, field));
    case FieldDescriptor::CPPTYPE_INT32:
        return parpare_bind(bind, MYSQL_TYPE_LONG, reflection->GetInt32(*message, field));
    case FieldDescriptor::CPPTYPE_UINT32:
        return parpare_bind(bind, MYSQL_TYPE_LONG, reflection->GetUInt32(*message, field));
    case FieldDescriptor::CPPTYPE_INT64:
        return parpare_bind(bind, MYSQL_TYPE_LONGLONG, reflection->GetInt64(*message, field));
    case FieldDescriptor::CPPTYPE_UINT64:
        return parpare_bind(bind, MYSQL_TYPE_LONGLONG, reflection->GetUInt64(*message, field));
    case FieldDescriptor::CPPTYPE_ENUM:
        return parpare_bind(bind, MYSQL_TYPE_LONG, reflection->GetEnumValue(*message, field));
    case FieldDescriptor::CPPTYPE_BOOL:
        return parpare_bind(bind, MYSQL_TYPE_TINY, (uint8)reflection->GetBool(*message, field));
    case FieldDescriptor::CPPTYPE_STRING:
        if (field->type() == FieldDescriptor::TYPE_BYTES)
            return parpare_bind(bind, MYSQL_TYPE_BLOB, reflection->GetString(*message, field));
        else
            return parpare_bind(bind, MYSQL_TYPE_VAR_STRING, reflection->GetString(*message, field));
    case FieldDescriptor::CPPTYPE_MESSAGE:
        {
            std::string output;
            const Message& submessage = reflection->GetMessage(*message, field);
            if (!submessage.SerializeToString(&output))
                return -1;
            return parpare_bind(bind, MYSQL_TYPE_BLOB, output);
        }
        break;
    default:
        log_error("parambuf::parpare_required field unknow type, field=%s", field->full_name().c_str());
        return -1;
    }
    return 0;
}

// TODO: check buffer capacity
int encode_single(CodedOutputStream* output, const Message* message, const FieldDescriptor* field, int index)
{
    const Reflection* reflection = message->GetReflection();
    switch (field->type())
    {
    case FieldDescriptor::TYPE_DOUBLE:
        WireFormatLite::WriteDoubleNoTag(reflection->GetRepeatedDouble(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_FLOAT:
        WireFormatLite::WriteFloatNoTag(reflection->GetRepeatedFloat(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_INT64:
        WireFormatLite::WriteInt64NoTag(reflection->GetRepeatedInt64(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_UINT64:
        WireFormatLite::WriteUInt64NoTag(reflection->GetRepeatedUInt64(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_INT32:
        WireFormatLite::WriteInt32NoTag(reflection->GetRepeatedInt32(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_UINT32:
        WireFormatLite::WriteUInt32NoTag(reflection->GetRepeatedUInt32(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_FIXED64:
        WireFormatLite::WriteFixed64NoTag(reflection->GetRepeatedUInt64(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_FIXED32:
        WireFormatLite::WriteFixed32NoTag(reflection->GetRepeatedUInt32(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_SFIXED32:
        WireFormatLite::WriteSFixed32NoTag(reflection->GetRepeatedInt32(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_SFIXED64:
        WireFormatLite::WriteSFixed64NoTag(reflection->GetRepeatedInt64(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_SINT32:
        WireFormatLite::WriteSInt32NoTag(reflection->GetRepeatedInt32(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_SINT64:
        WireFormatLite::WriteSInt64NoTag(reflection->GetRepeatedInt64(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_ENUM:
        WireFormatLite::WriteEnumNoTag(reflection->GetRepeatedEnumValue(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_BOOL:
        WireFormatLite::WriteBoolNoTag(reflection->GetRepeatedBool(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_STRING:
    case FieldDescriptor::TYPE_BYTES:
        WireFormatLite::WriteString(field->number(), reflection->GetRepeatedString(*message, field, index), output);
        break;
    case FieldDescriptor::TYPE_MESSAGE:
        {
            const Message& submessage = reflection->GetRepeatedMessage(*message, field, index);
            int size = submessage.ByteSizeLong();
            WireFormatLite::WriteMessage(field->number(), submessage, output);
        }
        break;
    default:
        return -1;
    }
    return 0;
}

int parambuf::parpare_repeated(const Message* message, const FieldDescriptor* field, MYSQL_BIND& bind)
{
    int size = buffer_ + length_ - current_;
    ArrayOutputStream buffer(current_, size);
    CodedOutputStream stream(&buffer);

    const Reflection* reflection = message->GetReflection();
    int field_size = reflection->FieldSize(*message, field);
    for (int index = 0; index < field_size; index++)
    {
        int ret = encode_single(&stream, message, field, index);
        if (ret != 0)            
        {
            log_error("parambuf::parpare_repeated encode field fail, field=%s", field->full_name().c_str());
            return ret;
        }
    }

    char* position = NULL;
    stream.GetDirectBufferPointer((void**)&position, &size);

    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = MYSQL_TYPE_BLOB;
    bind.buffer = current_;
    bind.buffer_length = position - current_;
    current_ = position;
    return 0;
}