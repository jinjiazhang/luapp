#include <stdio.h>
#include "plat.h"
#include "luapp.h"

luapp::luapp() : lobject(luaL_newstate())
{
    daemon_ = false;
    quit_ = false;
    app_mstime_ = 0;
    time_offset_ = 0;
}

luapp::~luapp()
{
    if (L != NULL)
    {
        lua_close(L);
        L = NULL;
    }
}

int64_t luapp::time()
{
    return (app_mstime_ + time_offset_) / 1000;
}

int64_t luapp::mstime()
{
    return app_mstime_ + time_offset_;
}

void luapp::offset(int64_t ms)
{
    time_offset_ += ms;
}

void luapp::run(luctx* ctx)
{
    entry_ = ctx->entry;
    daemon_ = ctx->daemon;

    if (ctx->daemon)
    {
        app_daemon();
    }

    if (init() != 0)
    {
        return;
    }

    while (true)
    {
        if (quit_)
        {
            if (quit() == 0)
            {
                break;
            }
        }
        if (proc() == 0)
        {
            idle();
        }
        tick();
    }
}

int luapp::init()
{
    app_mstime_ = app_mstime();
    luaL_openlibs(L);
    lua_pushlobject(L, this);
    lua_setglobal(L, "app");
    if (luaL_dofile(L, entry_.c_str()))
    {
        printf("%s\n", lua_tostring(L, -1));
        return -1;
    }

    app_mstime_ = app_mstime();
    luaL_callfunc(L, this, "init");
    return 0;
}

int luapp::proc()
{
    app_mstime_ = app_mstime();
    luaL_callfunc(L, this, "proc");
    return 0;
}

int luapp::tick()
{
    app_mstime_ = app_mstime();
    luaL_callfunc(L, this, "tick");
    return 0;
}

int luapp::idle()
{
    app_mstime_ = app_mstime();
    luaL_callfunc(L, this, "idle");
    app_sleep(1000);
    return 0;
}

int luapp::quit()
{
    app_mstime_ = app_mstime();
    luaL_callfunc(L, this, "quit");
    return 0;
}

EXPORT_OFUNC(luapp, time)
EXPORT_OFUNC(luapp, mstime)
EXPORT_OFUNC(luapp, offset)
const luaL_Reg* luapp::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(luapp, time) },
        { IMPORT_OFUNC(luapp, mstime) },
        { IMPORT_OFUNC(luapp, offset) },
        { NULL, NULL }
    };
    return libs;
}