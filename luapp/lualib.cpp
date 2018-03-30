#include "lualib.h"

bool luaL_pushfunc(lua_State* L, std::string* err, const char* name)
{
	lua_getglobal(L, name);
	return true;
}

bool luaL_pushfunc(lua_State* L, std::string* err, const char* module, const char* name)
{
	lua_getglobal(L, module);
	lua_getfield(L, -1, name);
	lua_remove(L, -2);
	return true;
}

bool luaL_safecall(lua_State* L, std::string* err, int nargs, int nrets)
{
	lua_pcall(L, nargs, nrets, 0);
	return true;
}