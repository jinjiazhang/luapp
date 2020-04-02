#include "luajson.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <string>

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

static void luaL_fillkey(lua_State* L, int index, rapidjson::Value& key, rapidjson::Document::AllocatorType& allocator)
{
    int type = lua_type(L, index);
    switch (type)
    {
    case LUA_TNUMBER:
        if (lua_isinteger(L, index))
            key.SetString(std::to_string(lua_tointeger(L, index)).c_str(), allocator);
        else
            key.SetString(std::to_string(lua_tonumber(L, index)).c_str(), allocator);
        break;
    case LUA_TSTRING:
        key.SetString(lua_tostring(L, index), allocator);
        break;
    default:
        break;
    }
}

static void luaL_fillvalue(lua_State* L, int index, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator)
{
    size_t length = 0;
    const char* bytes = nullptr;
    int type = lua_type(L, index);
    switch (type)
    {
    case LUA_TNUMBER:
        if (lua_isinteger(L, index))
            value.SetInt64(lua_tointeger(L, index));
        else
            value.SetDouble(lua_tonumber(L, index));
        break;
    case LUA_TBOOLEAN:
        value.SetBool(lua_toboolean(L, index) != 0);
        break;
    case LUA_TSTRING:
        bytes = lua_tolstring(L, index, &length);
        value.SetString(bytes, length);
        break;
    case LUA_TTABLE:
        if (luaL_isarray(L, index))
        {
            value.SetArray();
            int count = (int)luaL_len(L, index);
            for (int i = 0; i < count; i++)
            {
                lua_geti(L, index, i + 1);
                rapidjson::Value v;
                luaL_fillvalue(L, lua_absindex(L, -1), v, allocator);
                value.PushBack(v, allocator);
                lua_pop(L, 1);
            }
        }
        else
        {
            value.SetObject();
            lua_pushnil(L);
            while (lua_next(L, index))
            {
                rapidjson::Value k, v;
                luaL_fillkey(L, lua_absindex(L, -2), k, allocator);
                luaL_fillvalue(L, lua_absindex(L, -1), v, allocator);
                value.AddMember(k, v, allocator);
                lua_pop(L, 1);
            }
        }
        break;
    default:
        break;
    }
}

void luaL_pushvalue(lua_State* L, const rapidjson::Value& value)
{
    rapidjson::Value::ConstMemberIterator it;
    switch (value.GetType())
    {
    case rapidjson::kFalseType:
        lua_pushboolean(L, 0);
        break;
    case rapidjson::kTrueType:
        lua_pushboolean(L, 1);
        break;
    case rapidjson::kObjectType:
        lua_newtable(L);
        for (it = value.MemberBegin(); it != value.MemberEnd(); ++it)
        {
            luaL_pushvalue(L, it->name);
            luaL_pushvalue(L, it->value);
            lua_settable(L, -3);
        }
        break;
    case rapidjson::kArrayType:
        lua_newtable(L);
        for (unsigned index = 0; index < value.Size(); index++)
        {
            luaL_pushvalue(L, value[index]);
            lua_seti(L, -2, index + 1);
        }
        break;
    case rapidjson::kStringType:
        lua_pushstring(L, value.GetString());
        break;
    case rapidjson::kNumberType:
        if (value.IsDouble())
            lua_pushnumber(L, value.GetDouble());
        else
            lua_pushinteger(L, value.GetInt64());
        break;
    default:
        lua_pushnil(L);
        break;
    }
}

bool json_encode(lua_State* L, int index, bool pretty, char* output, size_t* size)
{
    rapidjson::Document document;
    luaL_fillvalue(L, index, document, document.GetAllocator());

    rapidjson::StringBuffer buffer;
    if (pretty)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
    }

    if (output && size) // export to buffer
    {
        JSON_ASSERT(*size >= buffer.GetLength());
        memcpy(output, buffer.GetString(), buffer.GetLength());
        *size = buffer.GetLength();
    }
    else
    {
        lua_pushlstring(L, buffer.GetString(), buffer.GetLength());
    }
    return true;
}

bool json_decode(lua_State* L, const char* input, size_t size)
{
    rapidjson::Document document;
    document.Parse(input, size);
    if (document.HasParseError())
    {
        json_error("json_decode parse error, code: %d", (int)document.GetParseError());
        return false;
    }

    luaL_pushvalue(L, document);
    return true;
}

static int encode(lua_State *L)
{
    bool pretty = false;
    if (lua_gettop(L) >= 2)
    {
        luaL_checktype(L, 2, LUA_TBOOLEAN);
        pretty = lua_toboolean(L, 2) != 0;
    }

    int stack = lua_gettop(L);
    if (!json_encode(L, 1, pretty, 0, 0))
    {
        json_error("json.encode fail, table=%s", lua_tostring(L, 1));
        return 0;
    }
    return lua_gettop(L) - stack;
}

static int decode(lua_State *L)
{
    assert(lua_gettop(L) == 1);
    luaL_checktype(L, 1, LUA_TSTRING);
    size_t size = 0;
    const char* data = lua_tolstring(L, 1, &size);

    if (!json_decode(L, data, size))
    {
        json_error("json.decode fail, json=%s", data);
        return 0;
    }
    return lua_gettop(L) - 1;
}

int luaopen_luajson(lua_State* L)
{
    static const struct luaL_Reg jsonLib[] = {
        {"encode", encode},
        {"decode", decode},
        {NULL, NULL}
    };

    lua_newtable(L);
    luaL_setfuncs(L, jsonLib, 0);
    lua_setglobal(L, "json");
    return 0;
}