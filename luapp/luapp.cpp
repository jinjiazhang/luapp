#include "plat.h"
#include "luapp.h"
#include "protolua/protolua.h"

luapp::luapp(lua_State* L) : lobject(L)
{
    ctx_ = nullptr;
    status_ = 0;
    svrid_ = 0;
    last_tick_ = 0;
    app_mstime_ = 0;
    time_offset_ = 0;

    network_ = nullptr;
    http_ = nullptr;
    timer_ = nullptr;
    luanet_ = nullptr;
	luaredis_ = nullptr;
    routermgr_ = nullptr;
}

luapp::~luapp()
{
    if (http_) 
        delete http_;
    if (timer_) 
        delete timer_;
    if (luanet_) 
        delete luanet_;
    if (luaredis_) 
        delete luaredis_;
    if (routermgr_)
        delete routermgr_;
    if (network_) 
        network_->release();
}

unsigned int luapp::svrid()
{
    return svrid_;
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

int luapp::status()
{
    return status_;
}

void luapp::change(int state)
{
    status_ = state;
}

void luapp::run(luctx* ctx)
{
    ctx_ = ctx;
    svrid_ = ctx_->svrid;

    if (ctx_->daemon)
    {
        app_daemon();
    }

    if (init() != 0)
    {
        return;
    }

    while (status_)
    {
        if (proc() == 0)
        {
            idle();
        }
        tick();
    }
}

void luapp::signal(int val)
{
    luaL_callfunc(L, this, "signal", val);
}

int luapp::init()
{
    app_mstime_ = sys_mstime();
    luaL_openlibs(L);
    luaopen_system(L);
    luaopen_protolog(L);
    luaopen_protolua(L);

    lua_pushlobject(L, this);
    lua_setglobal(L, "app");

    http_ = new lhttp(L);
    lua_pushlobject(L, http_);
    lua_setglobal(L, "http");

    timer_ = new ltimer(L, this->mstime());
    lua_pushlobject(L, timer_);
    lua_setglobal(L, "timer");

    network_ = create_network();
    luanet_ = new lnetwork(L, network_);
    lua_pushlobject(L, luanet_);
    lua_setglobal(L, "net");

	luaredis_ = new luaredis(L, network_);
	lua_pushlobject(L, luaredis_);
	lua_setglobal(L, "redis");

    routermgr_ = new routermgr(L, network_, svrid_);
    lua_pushlobject(L, routermgr_);
    lua_setglobal(L, "route");

    luaL_dostring(L, assist_code);
    if (!luaL_callfunc(L, this, "import", ctx_->entry))
    {
        return -1;
    }

    ++status_;
    app_mstime_ = sys_mstime();
    luaL_callfunc(L, this, "init");
    return 0;
}

int luapp::proc()
{
    app_mstime_ = sys_mstime();
    network_->update(ctx_->idle_sleep);
    http_->update();
    timer_->update(this->mstime());
    luaL_callfunc(L, this, "proc");

    int64_t cost_mstime = sys_mstime() - app_mstime_;
    if (cost_mstime >= ctx_->tick_invl)
    {
        return 1;
    }
    return 0;
}

int luapp::tick()
{
    app_mstime_ = sys_mstime();
    if (app_mstime_ - last_tick_ < ctx_->tick_invl)
    {
        return 1;
    }

    last_tick_ = app_mstime_;
    luaL_callfunc(L, this, "tick");
    return 0;
}

int luapp::idle()
{
    app_mstime_ = sys_mstime();
    luaL_callfunc(L, this, "idle");
    int64_t cost_mstime = sys_mstime() - app_mstime_;
    if (cost_mstime < ctx_->idle_sleep)
    {
        sys_sleep(ctx_->idle_sleep - (int)cost_mstime);
    }
    return 0;
}

EXPORT_OFUNC(luapp, svrid)
EXPORT_OFUNC(luapp, time)
EXPORT_OFUNC(luapp, mstime)
EXPORT_OFUNC(luapp, offset)
EXPORT_OFUNC(luapp, status)
EXPORT_OFUNC(luapp, change)
const luaL_Reg* luapp::get_libs()
{
    static const luaL_Reg libs[] = {
        { "init", lua_emptyfunc },
        { "proc", lua_emptyfunc },
        { "tick", lua_emptyfunc },
        { "idle", lua_emptyfunc },
        { IMPORT_OFUNC(luapp, svrid) },
        { IMPORT_OFUNC(luapp, time) },
        { IMPORT_OFUNC(luapp, mstime) },
        { IMPORT_OFUNC(luapp, offset) },
        { IMPORT_OFUNC(luapp, status) },
        { IMPORT_OFUNC(luapp, change) },
        { NULL, NULL }
    };
    return libs;
}