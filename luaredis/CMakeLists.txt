cmake_minimum_required(VERSION 3.1)
project(luaredis)

aux_source_directory(. DIR_SRCS)
file(GLOB_RECURSE DIR_INCS *.h *.hpp)
source_group("Include Files" FILES ${DIR_INCS})
add_library(${PROJECT_NAME} ${DIR_SRCS} ${DIR_INCS})

include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/lua-5.3.5/src)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/log4cplus/include)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/hiredis)
include_directories(${PROJECT_BINARY_DIR}/../thirdparty/log4cplus/include)