#include "luamysql.h"
#include "sqlpool.h"

using namespace google::protobuf;

luamysql::luamysql(lua_State* L) : lobject(L), 
    importer_(&source_tree_, nullptr)
{
    source_tree_.MapPath("", "./");
    source_tree_.MapPath("", "./proto/");
}

luamysql::~luamysql()
{
    for (sqlpool* pool : sqlpools_)
    {
        delete pool;
    }
}

int luamysql::update()
{
    int count = 0;
    for (sqlpool* pool : sqlpools_)
    {
        count += pool->update();
    }
    return count;
}

int luamysql::parse(lua_State* L)
{
    assert(lua_gettop(L) == 1);
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* file = lua_tostring(L, 1);

    if (!importer_.Import(file))
    {
        log_error("luamysql::parse fail, file=%s", file);
        return 0;
    }

    lua_pushboolean(L, true);
    return 1;
}

int luamysql::create_pool(lua_State* L)
{
    sqlpool* pool = new sqlpool(L);
    if (!pool->init(this))
    {
        delete pool;
        return 0;
    }

    sqlpools_.push_back(pool);
    lua_pushlobject(L, pool);
    return 1;
}

const Descriptor* luamysql::find_message(const char* proto)
{
    return importer_.pool()->FindMessageTypeByName(proto);
}

EXPORT_OFUNC(luamysql, parse)
EXPORT_OFUNC(luamysql, create_pool)
const luaL_Reg* luamysql::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(luamysql, parse) },
        { IMPORT_OFUNC(luamysql, create_pool) },
        { NULL, NULL }
    };
    return libs;
}