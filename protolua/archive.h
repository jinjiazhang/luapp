#ifndef _JINJIAZHANG_ARCHIVE_H_
#define _JINJIAZHANG_ARCHIVE_H_

#include "lua.hpp"
#include "protolog/protolog.h"

#ifdef _JINJIAZHANG_PROTOLOG_H_
#define archive_trace(fmt, ...)  log_trace(fmt, __VA_ARGS__)
#define archive_debug(fmt, ...)  log_debug(fmt, __VA_ARGS__)
#define archive_info(fmt, ...)   log_info(fmt, __VA_ARGS__)
#define archive_warn(fmt, ...)   log_warn(fmt, __VA_ARGS__)
#define archive_error(fmt, ...)  log_error(fmt, __VA_ARGS__)
#define archive_fatal(fmt, ...)  log_fatal(fmt, __VA_ARGS__)
#else
#define archive_trace(fmt, ...)
#define archive_debug(fmt, ...)
#define archive_info(fmt, ...)
#define archive_warn(fmt, ...)
#define archive_error(fmt, ...)
#define archive_fatal(fmt, ...)
#endif

#define ARCHIVE_DO(exp) { if(!(exp)) return false; }
#define ARCHIVE_ASSERT(exp) { if(!(exp)) return false; }

#define ARCHIVE_BUFFER_SIZE (1024 * 1024)

bool archive_pack(lua_State* L, int start, int end, char* output, size_t* size);
bool archive_unpack(lua_State* L, const char* input, size_t size);

int luaopen_archive(lua_State* L);

#endif