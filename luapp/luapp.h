#ifndef _JINJIAZHANG_LUAPP_H_
#define _JINJIAZHANG_LUAPP_H_

#include "lualib/lobject.h"
#include "luaredis/luaredis.h"
#include "luamongo/luamongo.h"
#include "protonet/lnetwork.h"
#include "proroute/routermgr.h"
#include "protosql/mysqlmgr.h"
#include "lhttp.h"
#include "ltimer.h"

struct luctx
{
    bool daemon;
    unsigned int svrid;
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
    unsigned int svrid();
    void exit();
    int64_t time();
    int64_t mstime();
    void offset(int64_t ms);

private:
    int init();
    int proc();
    int tick();
    int idle();

private:
    luctx* ctx_;
    int status_;
    unsigned int svrid_;
    int64_t last_tick_;
    int64_t app_mstime_;
    int64_t time_offset_;

    inetwork* network_;
    lhttp* http_;
    ltimer* timer_;
    lnetwork* luanet_;
	luaredis* luaredis_;
    luamongo* luamongo_;
    routermgr* routermgr_;
    mysqlmgr* mysqlmgr_;
};

extern const char* assist_code;
#endif