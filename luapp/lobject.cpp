#include "lobject.h"

lobject::lobject(lua_State* L)
{
	this->L = L;
	link_object();
}

lobject::~lobject()
{
	unlink_object();
	this->L = nullptr;
}

const char* lobject::meta_name()
{
	return typeid(*this).name();
}

const luaL_Reg* lobject::get_libs()
{
	static const luaL_Reg libs[] = {
		{ NULL, NULL }
	};
	return libs;
}

void lobject::link_object()
{
	// _R[this] = { __this = this }
	lua_pushlightuserdata(L, this);
	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "__this");
	lua_settable(L, LUA_REGISTRYINDEX);

	// setmetatable(_R[this], metatable)
	lua_pushlightuserdata(L, this);
	lua_gettable(L, LUA_REGISTRYINDEX);
	luaL_getmetatable(L, meta_name());
	if (lua_isnil(L, -1))
	{
		lua_remove(L, -1);
		luaL_newmetatable(L, meta_name());
		lua_newtable(L);
		luaL_setfuncs(L, get_libs(), 0);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

void lobject::unlink_object()
{
	// _R[this].__this = nil
	lua_pushlightuserdata(L, this);
	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushnil(L);
	lua_setfield(L, -2, "__this");
	lua_pop(L, 1);

	// _R[this] = nil
	lua_pushlightuserdata(L, this);
	lua_pushnil(L);
	lua_settable(L, LUA_REGISTRYINDEX);
}

void* lua_tolobject(lua_State* L, int idx)
{
	if (!lua_istable(L, idx)) {
		return nullptr;
	}
	
	lua_getfield(L, idx, "__this");
	if (!lua_isuserdata(L, -1)) {
		lua_pop(L, 1);
		return nullptr;
	}

	void* obj = lua_touserdata(L, -1);
	lua_pop(L, 1);
	return obj;
}

void lua_pushlobject(lua_State* L, void* obj)
{
	lua_pushlightuserdata(L, obj);
	lua_gettable(L, LUA_REGISTRYINDEX);
}