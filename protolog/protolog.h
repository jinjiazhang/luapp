#ifndef _JINJIAZHANG_PROTOLOG_H_
#define _JINJIAZHANG_PROTOLOG_H_

#include "log4cplus/log4cplus.h"

#pragma comment(lib, "log4cplus.lib")
#pragma comment(lib, "advapi32.lib")

using namespace log4cplus;
#define proto_trace(fmt, ...)   LOG4CPLUS_TRACE_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define proto_debug(fmt, ...)   LOG4CPLUS_DEBUG_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define proto_info(fmt, ...)    LOG4CPLUS_INFO_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define proto_warn(fmt, ...)    LOG4CPLUS_WARN_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define proto_error(fmt, ...)   LOG4CPLUS_ERROR_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define proto_fatal(fmt, ...)   LOG4CPLUS_FATAL_FMT(Logger::getRoot(), fmt, __VA_ARGS__)

#endif
