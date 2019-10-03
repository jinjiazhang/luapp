#include "mongoclient.h"

#define BSON_STATIC
#define MONGOC_STATIC
#include "mongoc/mongoc.h"

mongoclient::mongoclient(mongoc_client_pool_t* pool)
{

}

mongoclient::~mongoclient()
{

}

int mongoclient::command(const char* cmd)
{
    return 0;
}