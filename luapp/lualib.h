#ifndef _JINJIAZHANG_LUALIB_H_
#define _JINJIAZHANG_LUALIB_H_

#include <tuple>
#include <string>
#include "lua.hpp"
#define LUAPP_DO(exp) { if(!(exp)) return false; }

template <typename T> 
inline T luaL_getvalue(lua_State* L, int i) { return (T)lua_touserdata(L, i); }
template <> inline bool luaL_getvalue<bool>(lua_State* L, int i) { return lua_toboolean(L, i) != 0; }
template <> inline char luaL_getvalue<char>(lua_State* L, int i) { return (char)lua_tointeger(L, i); }
template <> inline unsigned char luaL_getvalue<unsigned char>(lua_State* L, int i) { return (unsigned char)lua_tointeger(L, i); }
template <> inline short luaL_getvalue<short>(lua_State* L, int i) { return (short)lua_tointeger(L, i); }
template <> inline unsigned short luaL_getvalue<unsigned short>(lua_State* L, int i) { return (unsigned short)lua_tointeger(L, i); }
template <> inline int luaL_getvalue<int>(lua_State* L, int i) { return (int)lua_tointeger(L, i); }
template <> inline unsigned int luaL_getvalue<unsigned int>(lua_State* L, int i) { return (unsigned int)lua_tointeger(L, i); }
template <> inline long luaL_getvalue<long>(lua_State* L, int i) { return (long)lua_tointeger(L, i); }
template <> inline unsigned long luaL_getvalue<unsigned long>(lua_State* L, int i) { return (unsigned long)lua_tointeger(L, i); }
template <> inline long long luaL_getvalue<long long>(lua_State* L, int i) { return lua_tointeger(L, i); }
template <> inline unsigned long long luaL_getvalue<unsigned long long>(lua_State* L, int i) { return (unsigned long long)lua_tointeger(L, i); }
template <> inline float luaL_getvalue<float>(lua_State* L, int i) { return (float)lua_tonumber(L, i); }
template <> inline double luaL_getvalue<double>(lua_State* L, int i) { return lua_tonumber(L, i); }
template <> inline const char* luaL_getvalue<const char*>(lua_State* L, int i) { return lua_tostring(L, i); }
template <> inline std::string luaL_getvalue<std::string>(lua_State* L, int i) { const char* str = lua_tostring(L, i); return str ? str : ""; }

template <typename T> 
inline void luaL_pushvalue(lua_State* L, T* v) { lua_pushlightuserdata(L, v); }
inline void luaL_pushvalue(lua_State* L, bool v) { lua_pushboolean(L, v); }
inline void luaL_pushvalue(lua_State* L, char v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned char v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, short v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned short v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, int v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned int v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, long v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned long v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, long long v) { lua_pushinteger(L, (lua_Integer)v); }
inline void luaL_pushvalue(lua_State* L, unsigned long long v) { lua_pushinteger(L, (lua_Integer)v); }
inline void luaL_pushvalue(lua_State* L, float v) { lua_pushnumber(L, v); }
inline void luaL_pushvalue(lua_State* L, double v) { lua_pushnumber(L, v); }
inline void luaL_pushvalue(lua_State* L, const char* v) { lua_pushstring(L, v); }
inline void luaL_pushvalue(lua_State* L, const std::string& v) { lua_pushstring(L, v.c_str()); }

template <size_t... ints>
struct luapp_sequence { };

template <size_t size, size_t... ints>
struct make_luapp_sequence : make_luapp_sequence<size - 1, size - 1, ints...> { };

template <size_t... ints>
struct make_luapp_sequence<0, ints...> : luapp_sequence<ints...> { };

const char* luaL_lasterr(lua_State* L);

bool luaL_pushfunc(lua_State* L, const char* name);

bool luaL_pushfunc(lua_State* L, const char* module, const char* name);

bool luaL_safecall(lua_State* L, int nargs, int nrets);

template<typename ...types>
inline bool luaL_pushargs(lua_State* L, types ...args)
{
	int _[] = { 0, (luaL_pushvalue(L, args), 0)... };
	return true;
}

template<size_t ...ints, typename ...types>
inline bool luaL_getrets(lua_State* L, std::tuple<types&...>& rets, luapp_sequence<ints...>&&)
{
	int _[] = { 0, (std::get<ints>(rets) = luaL_getvalue<types>(L, ints - sizeof...(ints)), 0)... };
	return true;
}

template<typename ...TTT, typename ...types>
inline bool luaL_callfunc(lua_State* L, std::tuple<TTT&...>&& rets, types ...args)
{
	LUAPP_DO(luaL_pushargs(L, args...));
	LUAPP_DO(luaL_safecall(L, sizeof...(types), sizeof...(TTT)));
	LUAPP_DO(luaL_getrets(L, rets, make_luapp_sequence<sizeof...(TTT)>()));
	return true;
}

template<typename ...TTT, typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* func, std::tuple<TTT&...>&& rets, types ...args)
{
	LUAPP_DO(luaL_pushfunc(L, func));
	LUAPP_DO(luaL_pushargs(L, args...));
	LUAPP_DO(luaL_safecall(L, sizeof...(types), sizeof...(TTT)));
	LUAPP_DO(luaL_getrets(L, rets, make_luapp_sequence<sizeof...(TTT)>()));
	return true;
}

template<typename ...TTT, typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* module, const char* func, std::tuple<TTT&...>&& rets, types ...args)
{
	LUAPP_DO(luaL_pushfunc(L, module, func));
	LUAPP_DO(luaL_pushargs(L, args...));
	LUAPP_DO(luaL_safecall(L, sizeof...(types), sizeof...(TTT)));
	LUAPP_DO(luaL_getrets(L, rets, std::make_luapp_sequence<sizeof...(TTT)>()));
	return true;
}

template<typename ...types>
inline bool luaL_callfunc(lua_State* L, types ...args) { return luaL_callfunc(L, std::tie(), args...); }

template<typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* func, types ...args) { return luaL_callfunc(L, func, std::tie(), args...);  }

template<typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* module, const char* func, types ...args) { return luaL_callfunc(L, module, func, std::tie(), args...); }
#endif