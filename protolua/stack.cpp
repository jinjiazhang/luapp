#include "protolua.h"

using namespace google::protobuf;
typedef unsigned int len_t;

bool pack_message(lua_State* L, int start, int end, char* output, size_t* size)
{
    size_t len = 0;
    const char* proto = lua_tolstring(L, start, &len);
    if (*size < len + 1 + sizeof(len_t))
    {
        proto_error("pack_message buffer limit, proto=%s", proto);
        return false;
    }

    memcpy(output, proto, len);
    output[len + 1] = '\0';

    len_t* data_len = (len_t*)(output + len + 1);
    output += len + 1 + sizeof(len_t);
    *size -= len + 1 + sizeof(len_t);
    
    if (!proto_pack(proto, L, start + 1, end, output, size))
    {
        log_error("pack_message fail, proto=%s", proto);
        return false;
    }

    *data_len = *size;
    *size = len + 1 + sizeof(len_t) + *data_len;
    return true;
}

bool unpack_message(lua_State* L, const char* input, size_t* size)
{
    const char* proto = input;
    int len = std::strlen(input);
    
    len_t data_len = *(len_t*)(input + len + 1);
    input += len + 1 + sizeof(len_t);
    size_t msg_size = len + 1 + sizeof(len_t) + data_len;
    if (*size < msg_size)
    {
        proto_error("unpack_message buffer limit, proto=%s", proto);
        return false;
    }

    int top = lua_gettop(L);
    lua_pushlstring(L, proto, len);
    if (!proto_unpack(proto, L, input, data_len))
    {
        lua_settop(L, top);
        return false;
    }

    *size = msg_size;
    return true;
}

// proto1, '0', data_len, data, proto2, '0', ...
bool stack_pack(lua_State* L, int start, int end, char* output, size_t* size)
{
    int total_size = 0;
    int left_size = *size;
    while (start <= end)
    {
        PROTO_ASSERT(lua_type(L, start) == LUA_TSTRING);
        const char* proto = lua_tostring(L, start);
        const Descriptor* descriptor = g_importer.pool()->FindMessageTypeByName(proto);
        PROTO_ASSERT(descriptor);
        int limit = std::min(end, start + descriptor->field_count());

        size_t msg_size = left_size;
        PROTO_DO(pack_message(L, start, limit, output, &msg_size));

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