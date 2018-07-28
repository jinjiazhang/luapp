cmake_minimum_required(VERSION 3.1)
project(luapp)

aux_source_directory(. DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

include_directories(../)
include_directories(../include)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(${PROJECT_NAME} lua lualib protonet protolua)