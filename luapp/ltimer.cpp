#include "ltimer.h"

ltimer::ltimer(lua_State* L, int64_t curtime) : lobject(L)
{
}

ltimer::~ltimer()
{
}

void ltimer::timeout(int tid)
{
    luaL_callfunc(L, this, "timeout", tid);
}

int ltimer::update(int64_t curtime)
{
    return timer_->update((time_t)curtime);
}

int ltimer::insert(int second)
{
    return timer_->insert(second, this);
}

bool ltimer::remove(int tid)
{
    return timer_->remove(tid);
}

bool ltimer::change(int tid, int second)
{
    return timer_->change(tid, second);
}

EXPORT_OFUNC(timer, insert)
EXPORT_OFUNC(timer, remove)
EXPORT_OFUNC(timer, change)
const luaL_Reg* ltimer::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(timer, insert) },
        { IMPORT_OFUNC(timer, remove) },
        { IMPORT_OFUNC(timer, change) },
        { NULL, NULL }
    };
    return libs;
}