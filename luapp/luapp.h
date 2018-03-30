#ifndef _JINJIAZHANG_LUAPP_H_
#define _JINJIAZHANG_LUAPP_H_

#include "lualib.h"

struct luctx
{
    std::string entry;
    bool daemon;
};

class luapp
{
public:
    luapp();
    ~luapp();

    void run(luctx* ctx);

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
    lua_State* L;
};

#endif