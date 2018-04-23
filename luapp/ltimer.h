#ifndef _JINJIAZHANG_LTIMER_H_
#define _JINJIAZHANG_LTIMER_H_

#include "lualib/lobject.h"
#include "timer.h"

class ltimer : public lobject, public timer::callback
{
public:
    ltimer(lua_State* L, int64_t current);
    ~ltimer();

public:
    virtual void timeout(int tid);
    virtual const luaL_Reg* get_libs();
    
    int update(int64_t current);
    int insert(int second);
    bool remove(int tid);
    bool change(int tid, int second);

private:
    timer* timer_;
};

#endif
