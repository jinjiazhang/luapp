#include "archive.h"
#include "varint.h"
#include <assert.h>

bool archive_pack(lua_State* L, int start, int end, char* output, size_t* size)
{
    return true;
}

bool archive_unpack(lua_State* L, const char* input, size_t size)
{
    return true;
}

// data = archive.pack(name, id, email)
static int pack(lua_State *L)
{
    assert(lua_gettop(L) >= 1);
    int stack = lua_gettop(L);
    if (!archive_pack(L, 1, stack, 0, 0))
    {
        archive_error("archive.unpack fail, first=%s", lua_tostring(L, 1));
        return 0;
    }

    return lua_gettop(L) - stack;
}

// name, id, email = archive.unpack(data)
static int unpack(lua_State *L)
{
    assert(lua_gettop(L) == 1);
    size_t size = 0;
    luaL_checktype(L, 1, LUA_TSTRING);
    const char* data = lua_tolstring(L, 1, &size);
    if (!archive_unpack(L, data, size))
    {
        archive_error("archive.unpack fail, data=%s", lua_tostring(L, 1));
        return 0;
    }

    return lua_gettop(L) - 2;
}

int luaopen_archive(lua_State* L)
{
    static const struct luaL_Reg archiveLib[] = {
        {"pack", pack},
        {"unpack", unpack},
        {NULL, NULL}
    };

    lua_newtable(L);
    luaL_setfuncs(L, archiveLib, 0);
    lua_setglobal(L, "archive");
    return 0;
}