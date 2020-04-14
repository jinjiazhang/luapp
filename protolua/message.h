#ifndef _JINJIAZHANG_MESSAGE_H_
#define _JINJIAZHANG_MESSAGE_H_

#include "lua.hpp"

#define MESSAGE_BUFFER_SIZE (1024 * 1024)
extern char msg_buf[MESSAGE_BUFFER_SIZE];

bool message_pack(lua_State* L, int start, int end, char* output, size_t* size);
bool message_unpack(lua_State* L, const char* input, size_t size);

#endif