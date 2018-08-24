#ifndef _JINJIAZHANG_LUAPP_H_
#define _JINJIAZHANG_LUAPP_H_

#include "lualib/lobject.h"
#include "protonet/lnetwork.h"
#include "lhttp.h"
#include "ltimer.h"

struct luctx
{
    bool daemon;
    const char* entry;
    const char* log_conf;
    int tick_freq;
    int tick_invl;
    int idle_sleep;
};

class luapp : public lobject
{
public:
    luapp(lua_State* L);
    ~luapp();

    void run(luctx* ctx);
    void signal(int val);
    virtual const luaL_Reg* get_libs();

public:
    int64_t time();
    int64_t mstime();
    void offset(int64_t ms);
    int status();
    void change(int state);

private:
    int init();
    int proc();
    int tick();
    int idle();

private:
    luctx* ctx_;
    int status_;
    int64_t last_tick_;
    int64_t app_mstime_;
    int64_t time_offset_;

    lhttp* http_;
    ltimer* timer_;
    lnetwork* network_;
};

extern const char* assist_code;
#endif