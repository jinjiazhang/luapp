#ifndef _JINJIAZHANG_ARGSBUF_H_
#define _JINJIAZHANG_ARGSBUF_H_

#include "lualib/lualib.h"

class argsbuf
{
public:
    argsbuf(int length);
    ~argsbuf();

public:
    int make_select(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);
    int make_insert(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);
    int make_update(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);
    int make_delete(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);
    int make_command(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);
    int make_increase(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens);

private:
    int length_;
    char* buffer_;
    char* current_;
};

#endif
