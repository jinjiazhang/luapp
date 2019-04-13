#ifndef _JINJIAZHANG_PROTOLOG_H_
#define _JINJIAZHANG_PROTOLOG_H_

#include "log4cplus/log4cplus.h"

// #pragma comment(lib, "log4cplus.lib")
// #pragma comment(lib, "advapi32.lib")

using namespace log4cplus;

#define put_trace(txt)        LOG4CPLUS_TRACE_STR(Logger::getRoot(), txt)
#define put_debug(txt)        LOG4CPLUS_DEBUG_STR(Logger::getRoot(), txt)
#define put_info(txt)         LOG4CPLUS_INFO_STR(Logger::getRoot(), txt)
#define put_warn(txt)         LOG4CPLUS_WARN_STR(Logger::getRoot(), txt)
#define put_error(txt)        LOG4CPLUS_ERROR_STR(Logger::getRoot(), txt)
#define put_fatal(txt)        LOG4CPLUS_FATAL_STR(Logger::getRoot(), txt)

#define log_trace(fmt, ...)   LOG4CPLUS_TRACE_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define log_debug(fmt, ...)   LOG4CPLUS_DEBUG_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define log_info(fmt, ...)    LOG4CPLUS_INFO_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define log_warn(fmt, ...)    LOG4CPLUS_WARN_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define log_error(fmt, ...)   LOG4CPLUS_ERROR_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define log_fatal(fmt, ...)   LOG4CPLUS_FATAL_FMT(Logger::getRoot(), fmt, __VA_ARGS__)

#endif
