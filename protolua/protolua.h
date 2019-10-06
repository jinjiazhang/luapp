#ifndef _JINJIAZHANG_PROTO_H_
#define _JINJIAZHANG_PROTO_H_

#include "lua.hpp"
#include "protolog/protolog.h"

#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/compiler/importer.h"

// #pragma comment(lib, "protobuf.lib")

#ifdef _JINJIAZHANG_PROTOLOG_H_
#define proto_trace(fmt, ...)  log_trace(fmt, __VA_ARGS__)
#define proto_debug(fmt, ...)  log_debug(fmt, __VA_ARGS__)
#define proto_info(fmt, ...)   log_info(fmt, __VA_ARGS__)
#define proto_warn(fmt, ...)   log_warn(fmt, __VA_ARGS__)
#define proto_error(fmt, ...)  log_error(fmt, __VA_ARGS__)
#define proto_fatal(fmt, ...)  log_fatal(fmt, __VA_ARGS__)
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
#define PROTO_BUFFER_SIZE (1024 * 1024)

bool proto_parse(const char* file, lua_State* L);
bool proto_encode(const char* proto, lua_State* L, int index, char* output, size_t* size);
bool proto_decode(const char* proto, lua_State* L, const char* input, size_t size);
bool proto_pack(const char* proto, lua_State* L, int start, int end, char* output, size_t* size);
bool proto_unpack(const char* proto, lua_State* L, const char* input, size_t size);
bool proto_tojson(lua_State* L, int index, bool pretty, char* output, size_t* size);
bool proto_fromjson(lua_State* L, const char* input, size_t size);
bool message_pack(lua_State* L, int start, int end, char* output, size_t* size);
bool message_unpack(lua_State* L, const char* input, size_t size);

extern google::protobuf::compiler::Importer g_importer;
extern google::protobuf::DynamicMessageFactory g_factory;
int luaopen_protolua(lua_State* L);

#endif