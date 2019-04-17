#include "protolua.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

bool proto_tojson(lua_State* L, int index, char* output, size_t* size)
{
    RAPIDJSON_NAMESPACE::Document doc;
    return true;
}

bool luaL_pushvalue(lua_State* L, const RAPIDJSON_NAMESPACE::Value& value)
{
    RAPIDJSON_NAMESPACE::Value::ConstMemberIterator it;
    switch (value.GetType())
    {
    case RAPIDJSON_NAMESPACE::kFalseType:
        lua_pushboolean(L, 0);
        break;
    case RAPIDJSON_NAMESPACE::kTrueType:
        lua_pushboolean(L, 1);
        break;
    case RAPIDJSON_NAMESPACE::kObjectType:
        lua_newtable(L);
        for (it = value.MemberBegin(); it != value.MemberEnd(); ++it)
        {
            PROTO_DO(luaL_pushvalue(L, it->name));
            PROTO_DO(luaL_pushvalue(L, it->value));
            lua_settable(L, -3);
        }
        break;
    case RAPIDJSON_NAMESPACE::kArrayType:
        lua_newtable(L);
        for (unsigned index = 0; index < value.Size(); index++)
        {
            PROTO_DO(luaL_pushvalue(L, value[index]));
            lua_seti(L, -2, index + 1);
        }
        break;
    case RAPIDJSON_NAMESPACE::kStringType:
        lua_pushstring(L, value.GetString());
        break;
    case RAPIDJSON_NAMESPACE::kNumberType:
        if (value.IsDouble())
            lua_pushnumber(L, value.GetDouble());
        else
            lua_pushinteger(L, value.GetInt64());
        break;
    default:
        lua_pushnil(L);
        break;
    }
    return true;
}

bool proto_fromjson(lua_State* L, const char* input, size_t size)
{
    RAPIDJSON_NAMESPACE::Document doc;
    doc.Parse(input, size);
    
    if (doc.HasParseError())
    {
        proto_error("proto_fromjson parse error: %s", doc.GetParseError());
        return false;
    }

    PROTO_DO(luaL_pushvalue(L, doc));
    return true;
}
