#include "archive.h"
#include "varint.h"
#include <assert.h>

enum class ar_type : unsigned char
{
    nil = 128,
    number,
    integer,
    string,
    bool_true,
    bool_false,
    table_head,
    table_tail,
};

bool pack_value(lua_State* L, int index, char* output, size_t* size);
bool unpack_value(lua_State* L, const char* input, size_t* size);

bool pack_nil(char* output, size_t* size)
{
    size_t in_size = *size;
    if (in_size < sizeof(unsigned char))
        return false;
    *output = (unsigned char)ar_type::nil;
    *size = sizeof(unsigned char);
    return true;
}

bool unpack_nil(lua_State* L, const char* input, size_t* size)
{
    size_t in_size = *size;
    assert(in_size >= sizeof(unsigned char));
    assert((ar_type)*input == ar_type::nil);
    lua_pushnil(L);
    *size = sizeof(unsigned char);
    return true;
}

bool pack_number(lua_State* L, int index, char* output, size_t* size)
{
    size_t in_size = *size;
    if (in_size < sizeof(unsigned char) + sizeof(double))
        return false;
    *output++ = (unsigned char)ar_type::number;
    double value = lua_tonumber(L, index);
    memcpy(output, &value, sizeof(double));
    *size = sizeof(unsigned char) + sizeof(double);
    return true;
}

bool unpack_number(lua_State* L, const char* input, size_t* size)
{
    size_t in_size = *size;
    assert(in_size >= sizeof(unsigned char));
    assert((ar_type)*input == ar_type::number);
    input += sizeof(unsigned char);
    double value = *(double*)input;
    lua_pushnumber(L, value);
    *size = sizeof(unsigned char) + sizeof(double);
    return true;
}

bool pack_integer(lua_State* L, int index, char* output, size_t* size)
{
    size_t in_size = *size;
    if (in_size < sizeof(unsigned char))
        return false;
    *output++ = (unsigned char)ar_type::integer;
    in_size -= sizeof(unsigned char);
    int64_t value = lua_tointeger(L, index);
    int varlen = encode_varint(output, in_size, zigzag_encode(value));
    if (varlen <= 0)
        return false;
    *size = sizeof(unsigned char) + varlen;
    return true;
}

bool unpack_integer(lua_State* L, const char* input, size_t* size)
{
    size_t in_size = *size;
    assert(in_size >= sizeof(unsigned char));
    assert((ar_type)*input == ar_type::integer);
    input += sizeof(unsigned char);
    in_size -= sizeof(unsigned char);
    uint64_t value;
    int varlen = decode_varint(&value, input, in_size);
    if (varlen <= 0)
        return false;
    lua_pushinteger(L, zigzag_decode(value));
    *size = sizeof(unsigned char) + varlen;
    return true;
}

bool pack_boolean(lua_State* L, int index, char* output, size_t* size)
{
    size_t in_size = *size;
    if (in_size < sizeof(unsigned char))
        return false;
    bool value = !!lua_toboolean(L, index);
    *output = (unsigned char)(value ? ar_type::bool_true : ar_type::bool_false);
    *size = sizeof(unsigned char);
    return true;
}

bool upack_boolean(lua_State* L, const char* input, size_t* size)
{
    size_t in_size = *size;
    assert(in_size >= sizeof(unsigned char));
    assert((ar_type)*input == ar_type::bool_true || (ar_type)*input == ar_type::bool_false);
    bool value = ((ar_type)*input == ar_type::bool_true);
    lua_pushboolean(L, value);
    *size = sizeof(unsigned char);
    return true;
}

bool pack_string(lua_State* L, int index, char* output, size_t* size)
{
    size_t in_size = *size;
    if (in_size < sizeof(unsigned char))
        return false;
    *output++ = (unsigned char)ar_type::string;
    in_size -= sizeof(unsigned char);
    size_t len = 0;
    const char* value = lua_tolstring(L, index, &len);
    int varlen = encode_varint(output, in_size, len);
    if (varlen <= 0)
        return false;
    output += varlen;
    in_size -= varlen;
    if (in_size < len)
        return false;
    memcpy(output, value, len);
    *size = sizeof(unsigned char) + varlen + len;
    return true;
}

bool unpack_string(lua_State* L, const char* input, size_t* size)
{
    size_t in_size = *size;
    assert(in_size >= sizeof(unsigned char));
    assert((ar_type)*input == ar_type::string);
    input += sizeof(unsigned char);
    in_size -= sizeof(unsigned char);
    uint64_t len;
    int varlen = decode_varint(&len, input, in_size);
    if (varlen <= 0)
        return false;
    input += varlen;
    in_size -= varlen;
    if (in_size < len)
        return false;
    lua_pushlstring(L, input, (size_t)len);
    *size = sizeof(unsigned char) + varlen + (size_t)len;
    return true;
}

bool pack_table(lua_State* L, int index, char* output, size_t* size)
{
    size_t in_size = *size;
    if (in_size < 4 * sizeof(unsigned char))
        return false;
    *output++ = (unsigned char)ar_type::table_head;
    unsigned char* lasize = (unsigned char*)output++;
    unsigned char* lhsize = (unsigned char*)output++;
    in_size -= 3 * sizeof(unsigned char);

    int count = 0;
    size_t total_size = 0;
    lua_pushnil(L);
    while (lua_next(L, index))
    {
        for (int i = -2; i < 0; i++)
        {
            size_t used = in_size;
            if (!pack_value(L, i, output, &used))
                return false;
            output += used;
            in_size -= used;
            total_size += used;
        }

        count++;
        lua_pop(L, 1);
    }

    if (in_size < sizeof(unsigned char))
        return false;
    *output++ = (unsigned char)ar_type::table_tail;
    *size = 4 * sizeof(unsigned char) + total_size;
    return true;
}

bool unpack_table(lua_State* L, const char* input, size_t* size)
{
    size_t in_size = *size;
    assert(in_size >= sizeof(unsigned char));
    assert((ar_type)*input == ar_type::table_head);
    if (in_size < 4 * sizeof(unsigned char))
        return false;
    const char* end = input + in_size;
    unsigned char* lasize = (unsigned char*)input + 1;
    unsigned char* lhsize = (unsigned char*)input + 2;
    input += 3 * sizeof(unsigned char);
    in_size -= 3 * sizeof(unsigned char);
    size_t total_size = 0;
    lua_newtable(L);
    while (input < end)
    {
        if ((ar_type)*input == ar_type::table_tail)
        {
            *size = 4 * sizeof(unsigned char) + total_size;
            return true;
        }
        for (int i = 0; i < 2; i++)
        {
            size_t used = in_size;
            if (!unpack_value(L, input, &used))
                return false;
            input += used;
            in_size -= used;
            total_size += used;
        }
        lua_settable(L, -3);
    }
    return false;
}

bool pack_value(lua_State* L, int index, char* output, size_t* size)
{
    index = lua_absindex(L, index);
    int type = lua_type(L, index);
    switch (type)
    {
    case LUA_TNIL:
        return pack_nil(output, size);
    case LUA_TNUMBER:
        return lua_isinteger(L, index)
            ? pack_integer(L, index, output, size)
            : pack_number(L, index, output, size);
    case LUA_TBOOLEAN:
        return pack_boolean(L, index, output, size);
    case LUA_TSTRING:
        return pack_string(L, index, output, size);
    case LUA_TTABLE:
        return pack_table(L, index, output, size);
    default:
        break;
    }
    return false;
}

bool unpack_value(lua_State* L, const char* input, size_t* size)
{
    assert(*size > 0);
    ar_type type = (ar_type)*input;
    switch (type)
    {
    case ar_type::nil:
        return unpack_nil(L, input, size);
    case ar_type::number:
        return unpack_number(L, input, size);
    case ar_type::integer:
        return unpack_integer(L, input, size);
    case ar_type::bool_true:
    case ar_type::bool_false:
        return upack_boolean(L, input, size);
    case ar_type::string:
        return unpack_string(L, input, size);
    case ar_type::table_head:
        return unpack_table(L, input, size);
    default:
        return false;
    }

    return true;
}

bool archive_pack(lua_State* L, int start, int end, char* output, size_t* size)
{
    start = lua_absindex(L, start);
    end = lua_absindex(L, end);

    size_t in_size = *size;
    size_t total_size = 0;
    for (int i = start; i <= end; i++)
    {
        size_t used = in_size;
        if (!pack_value(L, i, output, &used))
        {
            return false;
        }
        output += used;
        in_size -= used;
        total_size += used;
    }

    *size = total_size;
    return true;
}

bool archive_unpack(lua_State* L, const char* input, size_t size)
{
    int top = lua_gettop(L);
    while (size > 0)
    {
        size_t used = size;
        if (!unpack_value(L, input, &used))
        {
            lua_settop(L, top);
            return false;
        }
        input += used;
        size -= used;
    }
    return true;
}

static char buffer[ARCHIVE_BUFFER_SIZE];
// data = archive.pack(name, id, email)
static int pack(lua_State *L)
{
    assert(lua_gettop(L) >= 1);
    int stack = lua_gettop(L);
    size_t len = sizeof(buffer);
    if (!archive_pack(L, 1, stack, buffer, &len))
    {
        archive_error("archive.unpack fail, first=%s", lua_tostring(L, 1));
        return 0;
    }

    lua_pushlstring(L, buffer, len);
    return 1;
}

// name, id, email = archive.unpack(data)
static int unpack(lua_State *L)
{
    assert(lua_gettop(L) == 1);
    size_t size = 0;
    int stack = lua_gettop(L);
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* data = lua_tolstring(L, 1, &size);
    if (!archive_unpack(L, data, size))
    {
        archive_error("archive.unpack fail, data=%s", lua_tostring(L, 1));
        return 0;
    }

    return lua_gettop(L) - stack;
}

int luaopen_archive(lua_State* L)
{
    static const struct luaL_Reg archiveLib[] = {
        {"pack", pack},
        {"unpack", unpack},
        {NULL, NULL}
    };

    lua_newtable(L);
    luaL_setfuncs(L, archiveLib, 0);
    lua_setglobal(L, "archive");
    return 0;
}