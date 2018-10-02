cmake_minimum_required(VERSION 3.1)
project(luapp)

aux_source_directory(. DIR_SRCS)
file(GLOB_RECURSE DIR_INCS *.h *.hpp)
source_group("Include Files" FILES ${DIR_INCS}) 

add_executable(${PROJECT_NAME} ${DIR_SRCS} ${DIR_INCS})

include_directories(../)
include_directories(../thirdparty/include)
#link_libraries(../thirdparty/lib/lua.lib)
#link_libraries(../thirdparty/lib/curl.lib)
#link_libraries(../thirdparty/lib/log4cplus.lib)
#link_libraries(../thirdparty/lib/protobuf.lib)
#link_directories(../thirdparty/lib)
target_link_libraries(${PROJECT_NAME} lualib protonet protolua)