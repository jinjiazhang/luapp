#ifndef _JINJIAZHANG_LUAPP_H_
#define _JINJIAZHANG_LUAPP_H_

#include "lobject.h"

struct luctx
{
    const char* entry;
    bool daemon;
    int tick_freq;
    int tick_invl;
    int idle_sleep;
};

class luapp : public lobject
{
public:
    luapp();
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
};

#endif