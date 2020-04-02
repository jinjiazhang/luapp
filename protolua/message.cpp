#include "protolua.h"
#include "varint.h"

using namespace google::protobuf;

// [proto_len, proto, data]
bool message_pack(lua_State* L, int start, int end, char* output, size_t* size)
{
    start = lua_absindex(L, start);
    end = lua_absindex(L, end);
    luaL_checktype(L, start, LUA_TSTRING);

    int left_size = *size;
    size_t proto_len = 0;
    const char* proto = lua_tolstring(L, start, &proto_len);
    int head_len = encode_varint(output, left_size, proto_len);
    if (head_len < 0 || left_size < head_len + (int)proto_len)
    {
        proto_error("message_pack buffer limit, proto=%s", proto);
        return false;
    }

    memcpy(output + head_len, proto, proto_len);
    char* data = output + head_len + proto_len;
    size_t data_len = left_size - head_len - proto_len;
    if (!proto_pack(proto, L, start + 1, end, data, &data_len))
    {
        proto_error("message_pack pack proto failed, proto=%s", proto);
        return false;
    }

    *size = head_len + proto_len + data_len;
    return true;
}

// [proto, filed1, field2, ...]
bool message_unpack(lua_State* L, const char* input, size_t size)
{
    uint64_t proto_len = 0;
    int head_len = decode_varint(&proto_len, input, size);
    if (head_len <= 0)
    {
        proto_error("message_unpack decode head_len fail, size=%d", size);
        return false;
    }

    if ((int)size < head_len + proto_len)
    {
        proto_error("message_unpack buffer not enough proto, proto_len=%d", proto_len);
        return false;
    }

    int top = lua_gettop(L);
    std::string proto(input + head_len, (size_t)proto_len);
    lua_pushlstring(L, proto.data(), proto.size());
    input += head_len + (int)proto_len;
    size -= head_len + (int)proto_len;

    if (!proto_unpack(proto.c_str(), L, input, size))
    {
        lua_settop(L, top);
        proto_error("message_unpack unpack proto failed, proto=%s", proto.c_str());
        return false;
    }
    return true;
}