#include "producer.h"

producer::producer(lua_State* L) : lobject(L)
{

}

producer::~producer()
{

}

int producer::poll(lua_State* L)
{
    return 0;
}

int producer::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(producer, poll)
EXPORT_OFUNC(producer, close)
const luaL_Reg* producer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(producer, poll) },
        { IMPORT_OFUNC(producer, close) },
        { NULL, NULL }
    };
    return libs;
}