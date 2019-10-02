#include "lualib/lobject.h"
#ifndef _JINJIAZHANG_MONGOCLIENT_H_
#define _JINJIAZHANG_MONGOCLIENT_H_

#include "lualib/lobject.h"

class luamongo;
class mongoclient : public lobject
{

public:
    mongoclient(lua_State* L, luamongo* mongo);
	~mongoclient();

    int mongo_insert(lua_State* L);

    virtual const luaL_Reg* get_libs();

private:

};

#endif