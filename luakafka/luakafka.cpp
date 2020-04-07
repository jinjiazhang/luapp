#include "luakafka.h"

luakafka::luakafka(lua_State* L) : lobject(L)
{

}

luakafka::~luakafka()
{

}

int luakafka::producer(lua_State* L)
{
    return 0;
}

int luakafka::consumer(lua_State* L)
{
    return 0;
}

int luakafka::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(luakafka, producer)
EXPORT_OFUNC(luakafka, consumer)
EXPORT_OFUNC(luakafka, close)
const luaL_Reg* luakafka::get_libs()
{
    static const luaL_Reg libs[] = {
    	{ IMPORT_OFUNC(luakafka, producer) },
        { IMPORT_OFUNC(luakafka, consumer) },
        { IMPORT_OFUNC(luakafka, close) },
        { NULL, NULL }
    };
    return libs;
}