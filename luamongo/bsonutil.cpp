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
        bson_append_utf8(bson, key, key_length, bytes, length);
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
            lua_pushnil(L);
            while (lua_next(L, index))
            {
                size_t child_key_length = 0;
                const char* child_key = lua_tolstring(L, -2, &child_key_length);
                luaL_fillbson(L, lua_absindex(L, -1), &child, child_key, child_key_length);
                lua_pop(L, 1);
            }
            bson_append_document_end(bson, &child);
        }
        break;
    default:
        break;
    }
}

void luaL_pushvalue(lua_State* L, bson_iter_t& iter)
{
    const bson_value_t* value = bson_iter_value(&iter);
    switch (value->value_type)
    {
    case BSON_TYPE_OID:
    case BSON_TYPE_NULL:
        lua_pushnil(L);
        break;
    case BSON_TYPE_BOOL:
        lua_pushboolean(L, value->value.v_bool ? 1 : 0);
        break;
    case BSON_TYPE_INT32:
    case BSON_TYPE_TIMESTAMP:
        lua_pushinteger(L, value->value.v_int32);
        break;
    case BSON_TYPE_INT64:
        lua_pushinteger(L, value->value.v_int64);
        break;
    case BSON_TYPE_DOUBLE:
        lua_pushnumber(L, value->value.v_double);
        break;
    case BSON_TYPE_UTF8:
        lua_pushlstring(L, value->value.v_utf8.str, value->value.v_utf8.len);
        break;
    case BSON_TYPE_BINARY:
        lua_pushlstring(L, (char*)value->value.v_binary.data, value->value.v_binary.data_len);
        break;
    case BSON_TYPE_DOCUMENT:
        {
            bson_iter_t child;
            bson_iter_recurse(&iter, &child);

            lua_newtable(L);
            while (bson_iter_next(&child)) {
                const char* child_key = bson_iter_key(&child);
                int64_t num_key = bson_ascii_strtoll(child_key, nullptr, 10);
                if (errno != 0)
                    lua_pushstring(L, child_key);
                else
                    lua_pushinteger(L, num_key);
                luaL_pushvalue(L, child);
                lua_settable(L, -3);
            }
        }
        break;
    case BSON_TYPE_ARRAY:
        {
            bson_iter_t child;
            bson_iter_recurse(&iter, &child);

            lua_newtable(L);
            int index = 1;
            while (bson_iter_next(&child)) {
                luaL_pushvalue(L, child);
                lua_seti(L, -2, index++);
            }
        }
        break;
    default:
        luaL_error(L, "luaL_pushbson type(%d) not supported", value->value_type);
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

void luaL_pushbson(lua_State* L, const bson_t* bson)
{
    bson_iter_t iter;
    bson_iter_init(&iter, bson);

    lua_newtable(L);
    while (bson_iter_next(&iter)) {
        const char* key = bson_iter_key(&iter);
        int64_t num_key = bson_ascii_strtoll(key, nullptr, 10);
        if (errno != 0)
            lua_pushstring(L, key);
        else
            lua_pushinteger(L, num_key);
        luaL_pushvalue(L, iter);
        lua_settable(L, -3);
    }
}
