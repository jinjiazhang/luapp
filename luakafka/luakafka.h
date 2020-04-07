#ifndef _JINJIAZHANG_LUAKAFKA_H_
#define _JINJIAZHANG_LUAKAFKA_H_

#include "lualib/lobject.h"

struct inetwork;
class luakafka : public lobject
{
public:
    luakafka(lua_State* L);
    ~luakafka();

    int producer(lua_State* L);
    int consumer(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:

};

#endif