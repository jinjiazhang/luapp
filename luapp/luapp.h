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
    int64_t get_time();
    void mov_time(int64_t offset);

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
    int64_t app_time_;
    int64_t time_offset_;
};

#endif