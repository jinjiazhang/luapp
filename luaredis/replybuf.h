#ifndef _JINJIAZHANG_REPLYBUF_H_
#define _JINJIAZHANG_REPLYBUF_H_

#include "lualib/lualib.h"
#include "hiredis.h"

class replybuf
{
public:
    replybuf();
    ~replybuf();

public:
    int push_command(lua_State* L, redisReply* reply);
};

#endif
