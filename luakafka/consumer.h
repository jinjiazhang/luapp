#ifndef _JINJIAZHANG_CONSUMER_H_
#define _JINJIAZHANG_CONSUMER_H_

#include "lualib/lobject.h"

class consumer : public lobject
{
public:
    consumer(lua_State* L);
    ~consumer();

    bool init(std::map<std::string, std::string>& confs, std::string& errmsg);

    int subscribe(lua_State* L);
    int poll(lua_State* L);
    int close(lua_State* L);
    virtual const luaL_Reg* get_libs();

private:

};

#endif