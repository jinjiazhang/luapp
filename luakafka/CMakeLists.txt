cmake_minimum_required(VERSION 3.1)
project(luakafka)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" SUPPORT_CXX11)
if (NOT SUPPORT_CXX11)
    message(FATAL_ERROR "compiler not support c++11")
endif()

add_definitions(-std=c++11)
aux_source_directory(. DIR_SRCS)
file(GLOB_RECURSE DIR_INCS *.h *.hpp)
source_group("Include Files" FILES ${DIR_INCS}) 

add_definitions(-D LIBRDKAFKA_STATICLIB)
add_library(${PROJECT_NAME} ${DIR_SRCS} ${DIR_INCS})

include_directories(../)
include_directories(../thirdparty/lua-5.3.5/src)
include_directories(../thirdparty/log4cplus/include)
include_directories(../thirdparty/librdkafka/src)