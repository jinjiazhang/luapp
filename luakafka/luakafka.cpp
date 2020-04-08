#include "luakafka.h"
#include "producer.h"
#include "consumer.h"

luakafka::luakafka(lua_State* L) : lobject(L)
{

}

luakafka::~luakafka()
{
    for (producer* obj : producers_)
    {
        delete obj;
    }
    producers_.clear();

    for (consumer* obj : consumers_)
    {
        delete obj;
    }
    consumers_.clear();
}

int luakafka::update()
{
    int count = 0;
    for (producer* obj : producers_)
    {
        count += obj->update(0);
    }
    for (consumer* obj : consumers_)
    {
        count += obj->update(0);
    }
    return count;
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

    std::string errmsg;
    producer* obj = new producer(L, this);
    if (!obj->init(confs, errmsg))
    {
        delete obj;
        lua_pushnil(L);
        lua_pushlstring(L, errmsg.c_str(), errmsg.size());
        return 2;
    }

    producers_.push_back(obj);
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

    std::string errmsg;
    consumer* obj = new consumer(L, this);
    if (!obj->init(confs, errmsg))
    {
        delete obj;
        lua_pushnil(L);
        lua_pushlstring(L, errmsg.c_str(), errmsg.size());
        return 2;
    }

    consumers_.push_back(obj);
    luaL_pushvalue(L, obj);
    return 1;
}

void luakafka::destory_producer(producer* obj)
{
    std::vector<producer*>::iterator it = producers_.begin();
    for (; it != producers_.end(); ++it)
    {
        if (*it == obj)
        {
            producers_.erase(it);
            break;
        }
    }
    delete obj;
}

void luakafka::destory_consumer(consumer* obj)
{
    std::vector<consumer*>::iterator it = consumers_.begin();
    for (; it != consumers_.end(); ++it)
    {
        if (*it == obj)
        {
            consumers_.erase(it);
            break;
        }
    }
    delete obj;
}

int luakafka::close(lua_State* L)
{
    delete this;
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