#include "producer.h"

producer::producer(lua_State* L) : lobject(L)
{

}

producer::~producer()
{

}

bool producer::init(std::map<std::string, std::string>& confs)
{
    return true;
}

int producer::produce(lua_State* L)
{
    return 0;
}

int producer::poll(lua_State* L)
{
    return 0;
}

int producer::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(producer, produce)
EXPORT_OFUNC(producer, poll)
EXPORT_OFUNC(producer, close)
const luaL_Reg* producer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(producer, produce) },
        { IMPORT_OFUNC(producer, poll) },
        { IMPORT_OFUNC(producer, close) },
        { NULL, NULL }
    };
    return libs;
}