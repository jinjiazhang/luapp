#include "mongoclient.h"

#define BSON_STATIC
#define MONGOC_STATIC
#include "mongoc/mongoc.h"

mongoclient::mongoclient(lua_State* L, luamongo* mongo) : lobject(L)
{

}

mongoclient::~mongoclient()
{

}

int mongoclient::mongo_insert(lua_State* L)
{
    const char *uri_string = "mongodb://jinjiazh.com:27017";
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    bson_t *command, reply, *insert;
    bson_error_t error;
    char *str;
    bool retval;

    mongoc_init();

    lua_pushinteger(L, EXIT_SUCCESS);
    return 1;
}

EXPORT_OFUNC(mongoclient, mongo_insert)
const luaL_Reg* mongoclient::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(mongoclient, mongo_insert) },
        { NULL, NULL }
    };
    return libs;
}