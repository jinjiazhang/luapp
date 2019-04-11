cmake_minimum_required(VERSION 3.1)
project(hiredis_win)

add_library(hiredis 
    async.c
    hiredis.c
    net.c
    sds.c
)

add_library(win32_interop
    msvs/win32_interop/win32_common.cpp
    msvs/win32_interop/win32_fixes.c
    msvs/win32_interop/win32_ANSI.c
    msvs/win32_interop/win32_APIs.c
    msvs/win32_interop/win32_error.c
    msvs/win32_interop/win32_fdapi.cpp
    msvs/win32_interop/win32_fdapi_crt.cpp
    msvs/win32_interop/win32_rfdmap.cpp
    msvs/win32_interop/win32_variadic_functor.cpp
    msvs/win32_interop/win32_wsiocp.c
)