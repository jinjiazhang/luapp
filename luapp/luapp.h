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
    int init();
    int proc();
    int tick();
    int idle();
    int quit();

private:
	std::string entry_;
    bool daemon_;
    bool quit_;
};

#endif