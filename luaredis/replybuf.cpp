#include "replybuf.h"

replybuf::replybuf()
{

}

replybuf::~replybuf()
{

}

static void luaL_pushstatus(lua_State* L, redisReply* reply)
{
    if (reply == nullptr)
    {
        lua_pushinteger(L, -1);
    }
    else if (reply->type == REDIS_REPLY_ERROR)
    {
        lua_pushinteger(L, -2);
    }
    else
    {
        lua_pushinteger(L, 0);
    }
}

static void luaL_pushreply(lua_State* L, redisReply* reply)
{
    int reply_type = reply ? reply->type : 0;
    switch (reply_type)
    {
    case REDIS_REPLY_STRING:
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_ERROR:
        lua_pushlstring(L, reply->str, reply->len);
        break;
    case REDIS_REPLY_NIL:
        lua_pushnil(L);
        break;
    case REDIS_REPLY_INTEGER:
        lua_pushinteger(L, reply->integer);
        break;
    case REDIS_REPLY_ARRAY:
        lua_newtable(L);
        for (int i = 0; i < reply->elements; i++)
        {
            lua_pushinteger(L, i + 1);
            luaL_pushreply(L, reply->element[i]);
            lua_settable(L, -3);
        }
        break;
    default:
        lua_pushnil(L);
        break;
    }
}

int replybuf::push_command(lua_State* L, redisReply* reply)
{
    luaL_pushstatus(L, reply);
    luaL_pushreply(L, reply);
    return 0;
}