#ifndef _JINJIAZHANG_LHTTP_H_
#define _JINJIAZHANG_LHTTP_H_

#include "lualib/lobject.h"
#include "http.h"

class lhttp : public lobject, public http::callback
{
public:
    lhttp(lua_State* L);
    ~lhttp();

public:
    virtual void respond(int token, int code, const char* data);
    virtual const luaL_Reg* get_libs();
    
    int update();
    int get(const char* url);
    int post(const char* url, const char* data);

private:
    http* http_;
};

#endif
