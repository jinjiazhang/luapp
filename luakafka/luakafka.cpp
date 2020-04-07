#include "luakafka.h"
#include "producer.h"
#include "consumer.h"

luakafka::luakafka(lua_State* L) : lobject(L)
{

}

luakafka::~luakafka()
{

}

// kafka.create_producer(confs)
int luakafka::create_producer(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    std::map<std::string, std::string> confs;
    lua_pushnil(L);
    while (lua_next(L, 1))
    {
        std::string key = luaL_getvalue<std::string>(L, -2);
        std::string value = luaL_getvalue<std::string>(L, -1);
        confs.insert(std::make_pair(key, value));
        lua_pop(L, 1);
    }

    producer* obj = new producer(L);
    if (!obj->init(confs))
    {
        delete obj;
        return 0;
    }

    luaL_pushvalue(L, obj);
    return 1;
}

// kafka.create_consumer(confs)
int luakafka::create_consumer(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    std::map<std::string, std::string> confs;
    lua_pushnil(L);
    while (lua_next(L, 1))
    {
        std::string key = luaL_getvalue<std::string>(L, -2);
        std::string value = luaL_getvalue<std::string>(L, -1);
        confs.insert(std::make_pair(key, value));
        lua_pop(L, 1);
    }

    consumer* obj = new consumer(L);
    if (!obj->init(confs))
    {
        delete obj;
        return 0;
    }

    luaL_pushvalue(L, obj);
    return 1;
}

int luakafka::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(luakafka, create_producer)
EXPORT_OFUNC(luakafka, create_consumer)
EXPORT_OFUNC(luakafka, close)
const luaL_Reg* luakafka::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(luakafka, create_producer) },
        { IMPORT_OFUNC(luakafka, create_consumer) },
        { IMPORT_OFUNC(luakafka, close) },
        { NULL, NULL }
    };
    return libs;
}