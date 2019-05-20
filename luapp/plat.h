#ifndef _JINJIAZHANG_PLAT_H_
#define _JINJIAZHANG_PLAT_H_

#include "lualib/lualib.h"
#include "protolog/protolog.h"

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#else
using int64_t = long long;
using uint64_t = unsigned long long;
#endif

#ifdef _JINJIAZHANG_PROTOLOG_H_
#define luapp_trace(fmt, ...)  trace_fmt(fmt, __VA_ARGS__)
#define luapp_debug(fmt, ...)  debug_fmt(fmt, __VA_ARGS__)
#define luapp_info(fmt, ...)   info_fmt(fmt, __VA_ARGS__)
#define luapp_warn(fmt, ...)   warn_fmt(fmt, __VA_ARGS__)
#define luapp_error(fmt, ...)  error_fmt(fmt, __VA_ARGS__)
#define luapp_fatal(fmt, ...)  fatal_fmt(fmt, __VA_ARGS__)
#else
#define luapp_trace(fmt, ...)
#define luapp_debug(fmt, ...)
#define luapp_info(fmt, ...)
#define luapp_warn(fmt, ...)
#define luapp_error(fmt, ...)
#define luapp_fatal(fmt, ...)
#endif

// set window title
void set_title(unsigned int svrid, const char* entry);

// svrid string to int
unsigned int svrid_stoi(const std::string& text);

// svrid int to string
std::string svrid_itos(unsigned int svrid);

// sleep for millisecond time
void sys_sleep(int time);

// returns the millisecond since the 1970
int64_t sys_mstime();

// returns the time when the file last modify
int64_t sys_filetime(const char* file);

// returns the md5hex of the file
std::string sys_md5file(const char* file);

// concat string from lua stack
std::string lua_stackview(lua_State* L);

// fork in linux for daemon
void app_daemon();

int luaopen_system(lua_State* L);

int luaopen_protolog(lua_State* L);

#endif
