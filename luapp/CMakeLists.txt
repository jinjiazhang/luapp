cmake_minimum_required(VERSION 3.1)
project(luapp)

aux_source_directory(. DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

include_directories(../)
include_directories(../include)
link_libraries(../dep/lib/lua.lib)
link_libraries(../dep/lib/log4cplus.lib)
link_libraries(../dep/lib/protobuf.lib)
target_link_libraries(${PROJECT_NAME} lualib protonet protolua)