#include "bsonutil.h"

static bool luaL_isarray(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    int count = (int)luaL_len(L, index);
    if (count <= 0) return false;

    bool isarray = true;
    int stack = lua_gettop(L);

    lua_pushnil(L);
    while (lua_next(L, index))
    {
        int key_index = lua_absindex(L, -2);
        if (!lua_isinteger(L, key_index))
        {
            isarray = false;
            break;
        }

        if (lua_tointeger(L, key_index) > count)
        {
            isarray = false;
            break;
        }
        lua_pop(L, 1);
    }

    lua_settop(L, stack);
    return isarray;
}

static void luaL_fillbson(lua_State* L, int index, bson_t* bson, const char *key, int key_length)
{
    size_t length = 0;
    const char* bytes = nullptr;

    int type = lua_type(L, index);
    switch (type)
    {
    case LUA_TNUMBER:
        if (lua_isinteger(L, index))
            bson_append_int64(bson, key, key_length, lua_tointeger(L, index));
        else
            bson_append_double(bson, key, key_length, lua_tonumber(L, index));
        break;
    case LUA_TBOOLEAN:
        bson_append_bool(bson, key, key_length, lua_toboolean(L, index) != 0);
        break;
    case LUA_TSTRING:
        bytes = lua_tolstring(L, index, &length);
        bson_append_binary(bson, key, key_length, BSON_SUBTYPE_BINARY, (const uint8_t*)bytes, length);
        break;
    case LUA_TTABLE:
        if (luaL_isarray(L, index))
        {
            bson_t child;
            const char* child_key;
            char child_key_buf[16];
            size_t child_key_length;

            bson_append_array_begin(bson, key, key_length, &child);
            int count = (int)luaL_len(L, index);
            for (int i = 0; i < count; i++)
            {
                child_key_length = bson_uint32_to_string(i, &child_key, child_key_buf, sizeof(child_key_buf));
                lua_geti(L, index, i + 1);
                luaL_fillbson(L, lua_absindex(L, -1), &child, child_key, child_key_length);
                lua_pop(L, 1);
            }
            bson_append_array_end(bson, &child);
        }
        else
        {
            bson_t child;
            bson_append_document_begin(bson, key, key_length, &child);
            while (lua_next(L, index))
            {
                size_t length = 0;
                const char* key = lua_tolstring(L, -2, &length);
                luaL_fillbson(L, lua_absindex(L, -1), bson, key, key_length);
                lua_pop(L, 1);
            }
            bson_append_document_end(bson, &child);
        }
        break;
    default:
        break;
    }
}

bson_t* luaL_tobson(lua_State* L, int index)
{
    bson_t* bson = bson_new();
    lua_pushnil(L);
    while (lua_next(L, index))
    {
        size_t length = 0;
        const char* key = lua_tolstring(L, -2, &length);
        luaL_fillbson(L, lua_absindex(L, -1), bson, key, length);
        lua_pop(L, 1);
    }
    return bson;
}

void luaL_pushbson(lua_State* L, bson_t* bson)
{
    char* str = bson_as_json(bson, NULL);
    lua_pushstring(L, str);
}