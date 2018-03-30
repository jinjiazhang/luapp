#include "plat.h"
#include "luapp.h"
#include <stdio.h>

luapp::luapp()
{
    daemon_ = false;
    quit_ = false;
    L = NULL;
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
    L = luaL_newstate();
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