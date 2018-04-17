#ifndef _JINJIAZHANG_LUAPP_H_
#define _JINJIAZHANG_LUAPP_H_

#include "lobject.h"

struct luctx
{
    std::string entry;
    bool daemon;
};

class luapp : public lobject
{
public:
    luapp();
    ~luapp();

    void run(luctx* ctx);
    virtual const luaL_Reg* get_libs();

public:
    int64_t time();
    int64_t mstime();
    void offset(int64_t ms);

private:
    int init();
    int proc();
    int tick();
    int idle();
    int quit();

private:
    std::string entry_;
    bool daemon_;
    bool quit_;
    int64_t app_mstime_;
    int64_t time_offset_;
};

#endif