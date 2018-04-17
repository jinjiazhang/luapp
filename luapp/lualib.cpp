#include "lualib.h"

static std::string _last_err;
const char* luaL_lasterr(lua_State* L)
{
	return _last_err.c_str();
}

void luaL_seterr(lua_State* L, const char* format, ...)
{
	va_list	args;
	va_start(args, format);

	int length = _vscprintf(format, args);

	if (length > _last_err.capacity()) {
		_last_err.resize(length + 1);
	}

	vsprintf((char *)_last_err.c_str(), format, args);
	va_end(args);

	printf("%s\n", luaL_lasterr(L));
}

bool luaL_pushfunc(lua_State* L, const char* name)
{
	lua_getglobal(L, name);
	if (!lua_isfunction(L, -1)) {
		luaL_seterr(L, "luaL_pushfunc attempt to push a %s, name=%s", lua_typename(L, -1), name);
		lua_pop(L, 1);
		return false;
	}
	return true;
}

bool luaL_pushfunc(lua_State* L, void* obj, const char* name)
{
	lua_pushlightuserdata(L, obj);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if (!lua_istable(L, -1)) {
		luaL_seterr(L, "luaL_pushfunc attempt to push a %s, object=%p", lua_typename(L, -1), obj);
		lua_pop(L, 1);
		return false;
	}

	lua_getfield(L, -1, name);
	if (!lua_isfunction(L, -1)) {
		luaL_seterr(L, "luaL_pushfunc attempt to push a %s, object=%p, name=%s", lua_typename(L, -1), obj, name);
		lua_pop(L, 2);
		return false;
	}
	lua_remove(L, -2);
	return true;
}

bool luaL_pushfunc(lua_State* L, const char* module, const char* name)
{
	lua_getglobal(L, module);
	if (!lua_istable(L, -1)) {
		luaL_seterr(L, "luaL_pushfunc attempt to push a %s, module=%s", lua_typename(L, -1), module);
		lua_pop(L, 1);
		return false;
	}
		
	lua_getfield(L, -1, name);
	if (!lua_isfunction(L, -1)) {
		luaL_seterr(L, "luaL_pushfunc attempt to push a %s, module=%s, name=%s", lua_typename(L, -1), module, name);
		lua_pop(L, 2);
		return false;
	}
	lua_remove(L, -2);
	return true;
}

bool luaL_safecall(lua_State* L, int nargs, int nrets)
{
	int func = lua_gettop(L) - nargs;
	if (func <= 0 || !lua_isfunction(L, func)) {
		luaL_seterr(L, "luaL_safecall attempt to call a %s", lua_typename(L, func));
		return false;
	}

	luaL_pushfunc(L, "debug", "traceback");
	lua_insert(L, func);

	if (lua_pcall(L, nargs, nrets, func))
	{
		luaL_seterr(L, "luaL_safecall exception, %s", lua_tostring(L, -1));
		lua_pop(L, 2);
		return false;
	}
	lua_remove(L, -nrets - 1);
	return true;
}