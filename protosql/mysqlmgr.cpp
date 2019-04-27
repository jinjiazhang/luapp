#include "mysqlmgr.h"
#include "sqlclient.h"

using namespace google::protobuf;

mysqlmgr::mysqlmgr(lua_State* L) : lobject(L), 
    importer_(&source_tree_, nullptr)
{
    source_tree_.MapPath("", "./");
}

mysqlmgr::~mysqlmgr()
{
    
}

int mysqlmgr::parse(lua_State *L)
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

int mysqlmgr::connect(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* host = luaL_getvalue<const char*>(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* user = luaL_getvalue<const char*>(L, 2);
    luaL_checktype(L, 3, LUA_TSTRING);
    const char* passwd = luaL_getvalue<const char*>(L, 3);
    luaL_checktype(L, 4, LUA_TSTRING);
    const char* db = luaL_getvalue<const char*>(L, 4);
    luaL_checktype(L, 5, LUA_TNUMBER);
    unsigned int port = luaL_getvalue<int>(L, 5);

    sqlclient* client = new sqlclient();
    client->connect(host, user, passwd, db, port);

    const Descriptor* descriptor = importer_.pool()->FindMessageTypeByName("user");
    DynamicMessageFactory factory;
    const Message* prototype = factory.GetPrototype(descriptor);
    Message* message = prototype->New();
    const FieldDescriptor* id_field = descriptor->FindFieldByName("id");
    const FieldDescriptor* name_field = descriptor->FindFieldByName("name");
    const Reflection* reflection = message->GetReflection();
    reflection->SetString(message, id_field, "test003");
    reflection->SetString(message, name_field, "123321");
    client->sql_update(message, "id = 'test003'");
    client->sql_select(descriptor, "id = 'test003'");
    client->sql_delete(descriptor, "id = 'test003'");
    return 0;
}

EXPORT_OFUNC(mysqlmgr, parse)
EXPORT_OFUNC(mysqlmgr, connect)
const luaL_Reg* mysqlmgr::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(mysqlmgr, parse) },
        { IMPORT_OFUNC(mysqlmgr, connect) },
        { NULL, NULL }
    };
    return libs;
}