cmake_minimum_required(VERSION 3.1)
project(luapp)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" SUPPORT_CXX11)
if (NOT SUPPORT_CXX11)
    message(FATAL_ERROR "compiler not support c++11")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-E")
endif()

add_definitions(-std=c++11)
aux_source_directory(. DIR_SRCS)
file(GLOB_RECURSE DIR_INCS *.h *.hpp)
source_group("Include Files" FILES ${DIR_INCS}) 

add_executable(${PROJECT_NAME} ${DIR_SRCS} ${DIR_INCS})

include_directories(../)
include_directories(../thirdparty/lua-5.3.5/src)
include_directories(../thirdparty/log4cplus/include)
include_directories(../thirdparty/protobuf/src)
include_directories(../thirdparty/curl/include)


IF (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(${PROJECT_NAME} gateway proroute protonet protolua lualib crypto luaredis luamongo luamysql lua log4cplusS libprotobuf mariadbclient libcurl hiredis mongoc_static bson_static dl pthread)
ELSE ()
    target_link_libraries(${PROJECT_NAME} gateway proroute protonet protolua lualib crypto luaredis luamongo luamysql lua log4cplusS libprotobuf mariadbclient libcurl wldap32 hiredis mongoc_static bson_static)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Darwin")
