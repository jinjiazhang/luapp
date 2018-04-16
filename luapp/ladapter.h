#ifndef _JINJIAZHANG_LADAPTER_H_
#define _JINJIAZHANG_LADAPTER_H_

#include <functional>
#include "lualib.h"

#define EXPORT_CFUNC(func) \
int lua_##func(lua_State* L) \
{ \
	static cfunc f = make_luafunc(func); \
	return f(L); \
}

#define EXPORT_OFUNC(class, func) \
int lua_##class##_##func(lua_State* L) \
{ \
	static ofunc f = make_luafunc(&class::func); \
	class* obj = luaL_getvalue<class*>(L, 1); \
	if (obj == nullptr) { return 0; } \
	return f(obj, L); \
}

using cfunc = std::function<int(lua_State*)>;
using ofunc = std::function<int(void*, lua_State*)>;

template<size_t ...ints, typename T, typename ...types>
T call_cppfunc(lua_State* L, T(*func)(types...), luapp_sequence<ints...>&&)
{
	return (*func)(luaL_getvalue<types>(L, ints + 1)...);
}

template<size_t ...ints, typename T, class C, typename ...types>
T call_cppfunc(lua_State* L, C* obj, T(C::*func)(types...), luapp_sequence<ints...>&&)
{
	return (obj->*func)(luaL_getvalue<types>(L, ints + 1)...);
}

template<typename ...types>
cfunc make_luafunc(void(*func)(types...))
{
	return [=](lua_State* L)
	{
		call_cppfunc(L, func, make_luapp_sequence<sizeof...(types)>());
		return 0;
	};
}

template<typename T, typename ...types>
cfunc make_luafunc(T(*func)(types...))
{
	return [=](lua_State* L) 
	{
		T ret = call_cppfunc(L, func, make_luapp_sequence<sizeof...(types)>());
		luaL_pushvalue(L, ret);
		return 1;
	};
}

template<class C>
ofunc make_luafunc(int(C::*func)(lua_State*))
{
	return [=](void* obj, lua_State* L)
	{
		return (((C*)obj)->func)(L);
	};
}

template<class C, typename ...types>
ofunc make_luafunc(void(C::*func)(types...))
{
	return [=](void* obj, lua_State* L)
	{
		call_cppfunc(L, (C*)obj, func, make_luapp_sequence<sizeof...(types)>());
		return 0;
	};
}

template<class C, typename T, typename ...types>
ofunc make_luafunc(T(C::*func)(types...))
{
	return [=](void* obj, lua_State* L)
	{
		T ret = call_cppfunc(L, (C*)obj, func, make_luapp_sequence<sizeof...(types)>());
		luaL_pushvalue(L, ret);
		return 1;
	};
}

#endif
