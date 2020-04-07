#ifndef _JINJIAZHANG_CONSUMER_H_
#define _JINJIAZHANG_CONSUMER_H_

#include "lualib/lobject.h"

class consumer : public lobject
{
public:
    consumer(lua_State* L);
    ~consumer();

    int push(lua_State* L);
    int poll(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:

};

#endif