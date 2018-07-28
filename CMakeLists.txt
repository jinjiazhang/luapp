cmake_minimum_required(VERSION 3.1)
project(luapp)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#add_subdirectory(dep/lua-5.3.5/src)
#add_subdirectory(dep/log4cplus/src)
#add_subdirectory(dep/protobuf/src)

add_subdirectory(luapp)
add_subdirectory(lualib)
add_subdirectory(protolua)
add_subdirectory(protonet)