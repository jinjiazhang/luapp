#ifndef _JINJIAZHANG_BSONUTIL_H_
#define _JINJIAZHANG_BSONUTIL_H_

#include "lualib/lualib.h"
#include "mongoc/mongoc.h"

bson_t* luaL_tobson(lua_State* L, int index);
void luaL_pushbson(lua_State* L, const bson_t* bson);

#endif