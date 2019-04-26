#include "resultbuf.h"

resultbuf::resultbuf(int length)
{
    length_ = length;
    buffer_ = new char[length];
    current_ = buffer_;
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
}

int resultbuf::parpare(const google::protobuf::Descriptor* descriptor, std::vector<MYSQL_BIND>& binds)
{
    int field_count = descriptor->field_count();
    binds.resize(field_count);
    current_ = buffer_;

    int ret = 0;
    for (int i = 0; i < field_count; i++)
    {
        ret = parpare_field(descriptor->field(i), binds[i]);
        if (ret != 0)
        {
            break;
        }
    }

    return ret;
}

int resultbuf::parpare_field(const google::protobuf::FieldDescriptor* field, MYSQL_BIND& bind)
{
    return 0;
}

int resultbuf::parse(MYSQL_STMT* stmt, std::vector<MYSQL_BIND>& bind, google::protobuf::Message* message)
{
    return 0;
}