#include "bsonutil.h"

bson_t* luaL_tobson(lua_State* L, int index)
{
    bson_t* bson = bson_new();
    lua_pushnil(L);
    while (lua_next(L, index))
    {
        size_t length = 0;
        const char* key = lua_tolstring(L, -2, &length);
        bson_append_int32(bson, key, length, lua_tointeger(L, -1));
        lua_pop(L, 1);
    }
    return bson;
}

void luaL_pushbson(lua_State* L, bson_t* bson)
{
    char* str = bson_as_json(bson, NULL);
    lua_pushstring(L, str);
}