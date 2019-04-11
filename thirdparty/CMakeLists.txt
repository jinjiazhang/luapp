cmake_minimum_required(VERSION 3.1)
project(thirdparty)

option(CURL_STATICLIB "Set to ON to build libcurl with static linking." ON)
option(UNICODE "Build with tchar = wchar_t" OFF)
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "If TRUE, log4cplus is built as a shared library, otherwise as a static library")

add_subdirectory(lua-5.3.5)
add_subdirectory(curl-7.61.0)
add_subdirectory(log4cplus)
add_subdirectory(protobuf/cmake)