#include "consumer.h"

consumer::consumer(lua_State* L) : lobject(L)
{

}

consumer::~consumer()
{
    
}

int consumer::push(lua_State* L)
{
    return 0;
}

int consumer::poll(lua_State* L)
{
    return 0;
}

int consumer::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(consumer, push)
EXPORT_OFUNC(consumer, poll)
EXPORT_OFUNC(consumer, close)
const luaL_Reg* consumer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(consumer, push) },
        { IMPORT_OFUNC(consumer, poll) },
        { IMPORT_OFUNC(consumer, close) },
        { NULL, NULL }
    };
    return libs;
}