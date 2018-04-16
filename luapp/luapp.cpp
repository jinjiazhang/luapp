#include "plat.h"
#include "luapp.h"
#include <stdio.h>
#include "ladapter.h"

luapp::luapp() : lobject(luaL_newstate())
{
    daemon_ = false;
    quit_ = false;
}

luapp::~luapp()
{
    if (L != NULL)
    {
        lua_close(L);
        L = NULL;
    }
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
    luaL_openlibs(L);
    return 0;
}

int luapp::proc()
{
    return 0;
}

int luapp::tick()
{
    return 0;
}

int luapp::idle()
{
	app_sleep(1000);
    return 0;
}

int luapp::quit()
{
    return 0;
}

const luaL_Reg* luapp::get_libs()
{
	static const luaL_Reg libs[] = {
		{ NULL, NULL }
	};
	return libs;
}