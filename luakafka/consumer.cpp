#include "consumer.h"

consumer::consumer(lua_State* L, luakafka* kafka) : lobject(L)
{
    kafka_ = nullptr;
    rk_ = nullptr;
}

consumer::~consumer()
{
    if (rk_)
    {
        rd_kafka_flush(rk_, 3000);
        rd_kafka_destroy(rk_);
        rk_ = nullptr;
    }
}

bool consumer::init(std::map<std::string, std::string>& confs, std::string& errmsg)
{
    return true;
}

int consumer::update(int timeout)
{
    return 0;
}

int consumer::subscribe(lua_State* L)
{
    return 0;
}

int consumer::close(lua_State* L)
{
    return 0;
}

EXPORT_OFUNC(consumer, subscribe)
EXPORT_OFUNC(consumer, close)
const luaL_Reg* consumer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(consumer, subscribe) },
        { IMPORT_OFUNC(consumer, close) },
        { NULL, NULL }
    };
    return libs;
}