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
include_directories(../thirdparty/include)

find_library(LUA_LIB lua ../thirdparty/lib)
find_library(CURL_LIB curl ../thirdparty/lib)
find_library(LOG4CPLUS_LIB log4cplus ../thirdparty/lib)
find_library(PROTOBUF_LIB protobuf ../thirdparty/lib)



IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} protonet protolua lualib ${LUA_LIB} ${CURL_LIB} ${LOG4CPLUS_LIB} ${PROTOBUF_LIB} dl)
ELSE ()
    target_link_libraries(${PROJECT_NAME} protonet protolua lualib ${LUA_LIB} ${CURL_LIB} ${LOG4CPLUS_LIB} ${PROTOBUF_LIB})
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
