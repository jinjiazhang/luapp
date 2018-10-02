cmake_minimum_required(VERSION 3.1)
project(luapp)

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
target_link_libraries(${PROJECT_NAME} ${LUA_LIB} ${CURL_LIB} ${LOG4CPLUS_LIB} ${PROTOBUF_LIB} protolua lualib protonet)