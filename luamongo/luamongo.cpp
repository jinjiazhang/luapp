#include "luamongo.h"
#include "mongopool.h"
#include "mongoc/mongoc.h"

luamongo::luamongo(lua_State* L) : lobject(L)
{
    mongoc_init();
}

luamongo::~luamongo()
{
    mongoc_cleanup();
}

int luamongo::update()
{
    int count = 0;
    for (mongopool* pool : mongopools_)
    {
        count += pool->update();
    }
    return count;
}

int luamongo::create_pool(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* url = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int num = luaL_getvalue<int>(L, 2);

    mongopool* pool = new mongopool(this->L, this);
    if (!pool->init(url, num))
    {
        delete pool;
        return 0;
    }

    lua_pushlobject(L, pool);
    return 1;
}

EXPORT_OFUNC(luamongo, create_pool)
const luaL_Reg* luamongo::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(luamongo, create_pool) },
        { NULL, NULL }
    };
    return libs;
}