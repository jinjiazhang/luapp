#ifndef _JINJIAZHANG_PRODUCER_H_
#define _JINJIAZHANG_PRODUCER_H_

#include "lualib/lobject.h"

class producer : public lobject
{
public:
    producer(lua_State* L);
    ~producer();

    int poll(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:

};

#endif