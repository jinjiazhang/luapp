#ifndef _JINJIAZHANG_LUAMONGO_H_
#define _JINJIAZHANG_LUAMONGO_H_

#include "lualib/lobject.h"

class mongopool;
class luamongo : public lobject
{
public:
    luamongo(lua_State* L);
    ~luamongo();

    int update();
    int create_pool(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:
    std::vector<mongopool*> mongopools_;
};

#endif