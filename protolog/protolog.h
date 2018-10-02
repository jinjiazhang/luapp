#ifndef _JINJIAZHANG_PROTOLOG_H_
#define _JINJIAZHANG_PROTOLOG_H_

#include "log4cplus/log4cplus.h"

// #pragma comment(lib, "log4cplus.lib")
// #pragma comment(lib, "advapi32.lib")

using namespace log4cplus;

#define trace_str(txt)        LOG4CPLUS_TRACE_STR(Logger::getRoot(), txt)
#define debug_str(txt)        LOG4CPLUS_DEBUG_STR(Logger::getRoot(), txt)
#define info_str(txt)         LOG4CPLUS_INFO_STR(Logger::getRoot(), txt)
#define warn_str(txt)         LOG4CPLUS_WARN_STR(Logger::getRoot(), txt)
#define error_str(txt)        LOG4CPLUS_ERROR_STR(Logger::getRoot(), txt)
#define fatal_str(txt)        LOG4CPLUS_FATAL_STR(Logger::getRoot(), txt)

#define trace_fmt(fmt, ...)   LOG4CPLUS_TRACE_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define debug_fmt(fmt, ...)   LOG4CPLUS_DEBUG_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define info_fmt(fmt, ...)    LOG4CPLUS_INFO_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define warn_fmt(fmt, ...)    LOG4CPLUS_WARN_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define error_fmt(fmt, ...)   LOG4CPLUS_ERROR_FMT(Logger::getRoot(), fmt, __VA_ARGS__)
#define fatal_fmt(fmt, ...)   LOG4CPLUS_FATAL_FMT(Logger::getRoot(), fmt, __VA_ARGS__)

#endif
