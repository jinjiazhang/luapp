#include "ltimer.h"

ltimer::ltimer(lua_State* L, int64_t current) : lobject(L)
{
    timer_ = new timer(current);
}

ltimer::~ltimer()
{
    delete timer_;
}

void ltimer::timeout(int tid)
{
    luaL_callfunc(L, this, "timeout", tid);
}

int ltimer::update(int64_t current)
{
    return timer_->update(current);
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

EXPORT_OFUNC(ltimer, insert)
EXPORT_OFUNC(ltimer, remove)
EXPORT_OFUNC(ltimer, change)
const luaL_Reg* ltimer::get_libs()
{
    static const luaL_Reg libs[] = {
        { "timeout", lua_emptyfunc },
        { IMPORT_OFUNC(ltimer, insert) },
        { IMPORT_OFUNC(ltimer, remove) },
        { IMPORT_OFUNC(ltimer, change) },
        { NULL, NULL }
    };
    return libs;
}