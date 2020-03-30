#ifndef _JINJIAZHANG_LUAJSON_H_
#define _JINJIAZHANG_LUAJSON_H_

#include "lua.hpp"
#include "protolog/protolog.h"

#ifdef _JINJIAZHANG_PROTOLOG_H_
#define json_trace(fmt, ...)  log_trace(fmt, __VA_ARGS__)
#define json_debug(fmt, ...)  log_debug(fmt, __VA_ARGS__)
#define json_info(fmt, ...)   log_info(fmt, __VA_ARGS__)
#define json_warn(fmt, ...)   log_warn(fmt, __VA_ARGS__)
#define json_error(fmt, ...)  log_error(fmt, __VA_ARGS__)
#define json_fatal(fmt, ...)  log_fatal(fmt, __VA_ARGS__)
#else
#define json_trace(fmt, ...)
#define json_debug(fmt, ...)
#define json_info(fmt, ...)
#define json_warn(fmt, ...)
#define json_error(fmt, ...)
#define json_fatal(fmt, ...)
#endif

#define JSON_DO(exp) { if(!(exp)) return false; }
#define JSON_ASSERT(exp) { if(!(exp)) return false; }

bool json_encode(lua_State* L, int index, bool pretty, char* output, size_t* size);
bool json_decode(lua_State* L, const char* input, size_t size);

int luaopen_luajson(lua_State* L);

#endif