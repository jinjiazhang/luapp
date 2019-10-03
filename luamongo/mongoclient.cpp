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

    uri = mongoc_uri_new_with_error(uri_string, &error);
    if (!uri) {
        fprintf(stderr,
            "failed to parse URI: %s\n"
            "error message:       %s\n",
            uri_string,
            error.message);
        return 0;
    }

    /*
    * Create a new client instance
    */
    client = mongoc_client_new_from_uri(uri);
    if (!client) {
        return 0;
    }

    /*
    * Register the application name so we can track it in the profile logs
    * on the server. This can also be done from the URI (see other examples).
    */
    mongoc_client_set_appname(client, "connect-example");

    /*
    * Get a handle on the database "db_name" and collection "coll_name"
    */
    database = mongoc_client_get_database(client, "luapp");
    collection = mongoc_client_get_collection(client, "luapp", "game");

    /*
    * Do work. This example pings the database, prints the result as JSON and
    * performs an insert
    */
    command = BCON_NEW("ping", BCON_INT32(1));

    retval = mongoc_client_command_simple(
        client, "admin", command, NULL, &reply, &error);

    if (!retval) {
        fprintf(stderr, "%s\n", error.message);
        return 0;
    }

    str = bson_as_json(&reply, NULL);
    printf("%s\n", str);

    insert = BCON_NEW("hello", BCON_UTF8("world"));

    if (!mongoc_collection_insert_one(collection, insert, NULL, NULL, &error)) {
        fprintf(stderr, "%s\n", error.message);
    }

    bson_destroy(insert);
    bson_destroy(&reply);
    bson_destroy(command);
    bson_free(str);

    /*
    * Release our handles and clean up libmongoc
    */
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_uri_destroy(uri);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    lua_pushboolean(L, true);
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