#ifndef _JINJIAZHANG_LADAPTER_H_
#define _JINJIAZHANG_LADAPTER_H_

#include <functional>
#include "lualib.h"

using cppfunc = std::function<int(lua_State*)>;
using objfunc = std::function<int(void*, lua_State*)>;

template<size_t ...ints, typename T, typename ...types>
T call_cppfunc(lua_State* L, T(*func)(types...), luapp_sequence<ints...>&&)
{
	return (*func)(luaL_getvalue(L, ints + 1));
}

template<typename T, typename ...types>
cppfunc make_luafunc(T(*func)(types...))
{
	return [=](lua_State* L) 
	{
		T ret = call_cppfunc(L, func, make_luapp_sequence<sizeof...(types)>);
		luaL_pushvalue(L, ret);
		return 1;
	}
}

#endif
