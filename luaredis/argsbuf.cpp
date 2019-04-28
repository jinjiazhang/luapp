#include "argsbuf.h"

argsbuf::argsbuf()
{

}

argsbuf::~argsbuf()
{

}

int argsbuf::make_select(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens)
{
    return -1;
}

int argsbuf::make_insert(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens)
{
    return -1;
}

int argsbuf::make_update(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens)
{
    return -1;
}

int argsbuf::make_delete(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens)
{
    return -1;
}

int argsbuf::make_command(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens)
{
    int top = lua_gettop(L);
    args.resize(top);
    lens.resize(top);
    for (int i = 1; i <= top; i++)
    {
        size_t len = 0;
        const char* arg = lua_tolstring(L, i, &len);
        if (arg == nullptr || len == 0)
            return 0;
        args[i - 1] = arg;
        lens[i - 1] = len;
    }
    return 0;
}

int argsbuf::make_increase(lua_State* L, std::vector<const char*>& args, std::vector<size_t>& lens)
{
    return -1;
}