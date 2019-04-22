#include "protolua.h"
#include "varint.h"

using namespace google::protobuf;

bool pack_message(lua_State* L, Message* message, char* output, size_t* size)
{
    const std::string& proto = message->GetDescriptor()->name();
    int proto_len = (int)proto.size();
    int data_len = message->ByteSize();
    
    int head_len = length_varint(proto_len);
    int var_len = length_varint(data_len);
    int left_size = *size;

    if (left_size < head_len + proto_len + var_len + data_len)
    {
        proto_error("pack_message buffer limit, proto=%s", proto.c_str());
        return false;
    }

    PROTO_DO(encode_varint(output, left_size, proto_len));
    output += head_len;
    left_size -= head_len;

    memcpy(output, proto.data(), proto_len);
    output += proto_len;
    left_size -= proto_len;

    PROTO_DO(encode_varint(output, left_size, data_len));
    output += var_len;
    left_size -= var_len;

    uint8* start = reinterpret_cast<uint8*>(output);
    uint8* end = message->SerializeWithCachedSizesToArray((uint8*)output);
    if (end - start != data_len)
    {
        proto_error("pack_message serialize fail, proto=%s", proto.c_str());
        return false;
    }

    *size = head_len + proto_len + var_len + data_len;
    return true;
}

bool unpack_message(lua_State* L, const char* input, size_t* size)
{
    int left_size = *size;
    int proto_len = 0;
    int head_len = decode_varint(&proto_len, input, left_size);
    if (head_len <= 0)
    {
        proto_error("unpack_message decode head_len fail, left_size=%d", left_size);
        return false;
    }

    if (left_size < head_len + proto_len)
    {
        proto_error("unpack_message buffer not enough proto, proto_len=%d", proto_len);
        return false;
    }

    std::string proto(input + head_len, proto_len);
    input += head_len + proto_len;
    left_size -= head_len + proto_len;

    int data_len = 0;
    int var_len = decode_varint(&data_len, input, left_size);
    if (var_len <= 0)
    {
        proto_error("unpack_message decode varint fail, proto=%s", proto.c_str());
        return false;
    }

    if (left_size < var_len + data_len)
    {
        proto_error("unpack_message buffer limit, proto=%s", proto.c_str());
        return false;
    }

    input += var_len;
    left_size -= var_len;

    int top = lua_gettop(L);
    lua_pushlstring(L, proto.data(), proto.size());
    if (!proto_unpack(proto.c_str(), L, input, data_len))
    {
        lua_settop(L, top);
        return false;
    }

    *size = head_len + proto_len + var_len + data_len;
    return true;
}

bool encode_field(Message* message, const FieldDescriptor* field, lua_State* L, int index);
std::vector<const FieldDescriptor*> SortFieldsByNumber(const Descriptor* descriptor);

// proto1_len, proto1, data1_len, data1, proto2_len, proto2, ...
bool stack_pack(lua_State* L, int start, int end, char* output, size_t* size)
{
    int total_size = 0;
    int left_size = *size;

    start = lua_absindex(L, start);
    end = lua_absindex(L, end);

    while (start <= end)
    {
        PROTO_ASSERT(lua_type(L, start) == LUA_TSTRING);
        const char* proto = lua_tostring(L, start);

        const Descriptor* descriptor = g_importer.pool()->FindMessageTypeByName(proto);
        PROTO_ASSERT(descriptor);

        const Message* prototype = g_factory.GetPrototype(descriptor);
        PROTO_ASSERT(prototype);

        start += 1;
        int limit = std::min(end, start + descriptor->field_count());
        std::unique_ptr<Message> message(prototype->New());
        std::vector<const FieldDescriptor*> fields = SortFieldsByNumber(descriptor);
        for (int i = 0; i < (int)fields.size() && start + i <= limit; i++)
        {
            const FieldDescriptor* field = fields[i];
            PROTO_DO(encode_field(message.get(), field, L, start + i));
        }

        size_t msg_size = left_size;
        PROTO_DO(pack_message(L, message.get(), output, &msg_size));

        output += msg_size;
        left_size -= msg_size;
        total_size += msg_size;
        start = limit + 1;
    }

    *size = total_size;
    return true;
}

// proto1, filed1, field2, ..., proto2, ...
bool stack_unpack(lua_State* L, const char* input, size_t size)
{
    while (size > 0)
    {
        size_t msg_size = size;
        PROTO_DO(unpack_message(L, input, &msg_size));
        input += msg_size;
        size -= msg_size;
    }
    return true;
}