cmake_minimum_required(VERSION 3.1)
project(server)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs)

add_subdirectory(luapp)
add_subdirectory(lualib)
add_subdirectory(protolua)
add_subdirectory(protonet)