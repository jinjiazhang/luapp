#ifndef _JINJIAZHANG_PROTOLOG_H_
#define _JINJIAZHANG_PROTOLOG_H_

#define proto_info(fmt, ...)  proto_log(0, fmt, __VA_ARGS__)
#define proto_warn(fmt, ...)  proto_log(1, fmt, __VA_ARGS__)
#define proto_error(fmt, ...)  proto_log(2, fmt, __VA_ARGS__)
#define proto_fatal(fmt, ...)  proto_log(3, fmt, __VA_ARGS__)

void proto_log(int level, const char* fmt, ...);

#endif
