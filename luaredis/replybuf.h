#ifndef _JINJIAZHANG_REPLYBUF_H_
#define _JINJIAZHANG_REPLYBUF_H_

#include "lualib/lualib.h"
#include "hiredis.h"

class replybuf
{
public:
    replybuf(int length);
    ~replybuf();

public:
    int push_select(lua_State* L, redisReply* reply);
    int push_insert(lua_State* L, redisReply* reply);
    int push_update(lua_State* L, redisReply* reply);
    int push_delete(lua_State* L, redisReply* reply);
    int push_command(lua_State* L, redisReply* reply);
    int push_increase(lua_State* L, redisReply* reply);

private:

};

#endif
