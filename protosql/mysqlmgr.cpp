#include "mysqlmgr.h"
#include "sqlpool.h"

using namespace google::protobuf;

mysqlmgr::mysqlmgr(lua_State* L) : lobject(L), 
    importer_(&source_tree_, nullptr)
{
    source_tree_.MapPath("", "./");
}

mysqlmgr::~mysqlmgr()
{
    for (sqlpool* pool : sqlpools_)
    {
        delete pool;
    }
}

int mysqlmgr::update()
{
    for (sqlpool* pool : sqlpools_)
    {
        pool->update();
    }
    return 0;
}

int mysqlmgr::parse(lua_State* L)
{
    assert(lua_gettop(L) == 1);
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* file = lua_tostring(L, 1);

    if (!importer_.Import(file))
    {
        log_error("mysqlmgr::parse fail, file=%s", file);
        return 0;
    }

    lua_pushboolean(L, true);
    return 1;
}

int mysqlmgr::create_pool(lua_State* L)
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

const Descriptor* mysqlmgr::find_message(const char* proto)
{
    return importer_.pool()->FindMessageTypeByName(proto);
}

EXPORT_OFUNC(mysqlmgr, parse)
EXPORT_OFUNC(mysqlmgr, create_pool)
const luaL_Reg* mysqlmgr::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(mysqlmgr, parse) },
        { IMPORT_OFUNC(mysqlmgr, create_pool) },
        { NULL, NULL }
    };
    return libs;
}