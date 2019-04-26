#include "parambuf.h"
#include "protolog/protolog.h"

using namespace google::protobuf;


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
        // ret = parpare_field(field, binds[i]);
        if (ret != 0 || current_ > buffer_ + length_)
        {
            log_error("resultbuf::parpare parpare_field fail, name=%s", field->full_name().c_str());
            break;
        }
    }

    return ret;
}