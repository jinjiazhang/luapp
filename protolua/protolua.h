#ifndef _JINJIAZHANG_PROTO_H_
#define _JINJIAZHANG_PROTO_H_

#include "lua/lua.hpp"
#include "protolog/protolog.h"

#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/compiler/importer.h"

// #pragma comment(lib, "protobuf.lib")

#ifdef _JINJIAZHANG_PROTOLOG_H_
#define proto_trace(fmt, ...)  trace_fmt(fmt, __VA_ARGS__)
#define proto_debug(fmt, ...)  debug_fmt(fmt, __VA_ARGS__)
#define proto_info(fmt, ...)   info_fmt(fmt, __VA_ARGS__)
#define proto_warn(fmt, ...)   warn_fmt(fmt, __VA_ARGS__)
#define proto_error(fmt, ...)  error_fmt(fmt, __VA_ARGS__)
#define proto_fatal(fmt, ...)  fatal_fmt(fmt, __VA_ARGS__)
#else
#define proto_trace(fmt, ...)
#define proto_debug(fmt, ...)
#define proto_info(fmt, ...)
#define proto_warn(fmt, ...)
#define proto_error(fmt, ...)
#define proto_fatal(fmt, ...)
#endif

#define PROTO_DO(exp) { if(!(exp)) return false; }
#define PROTO_ASSERT(exp) { if(!(exp)) return false; }

using namespace google::protobuf;
using namespace google::protobuf::compiler;

bool ProtoParse(const char* file);
bool ProtoEncode(const char* proto, lua_State* L, int index, char* output, size_t* size);
bool ProtoDecode(const char* proto, lua_State* L, const char* input, size_t size);
bool ProtoPack(const char* proto, lua_State* L, int start, int end, char* output, size_t* size);
bool ProtoUnpack(const char* proto, lua_State* L, const char* input, size_t size);

extern Importer g_importer;
extern DynamicMessageFactory g_factory;
int luaopen_protolua(lua_State* L);

#endif