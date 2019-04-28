#ifndef _JINJIAZHANG_ARGSBUF_H_
#define _JINJIAZHANG_ARGSBUF_H_

#include "lualib/lualib.h"

class argsbuf
{
public:
    argsbuf();
    ~argsbuf();

public:
    int make_command(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);
};

#endif
