#include "protolog.h"
#include <windows.h>
#include <stdio.h>

void proto_log(int level, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
#ifdef _WIN32
    _vprintf_p(fmt, args);
#else
    vprintf(format, args);
#endif // _WIN32
    va_end(args);
}