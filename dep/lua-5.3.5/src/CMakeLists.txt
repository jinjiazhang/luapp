cmake_minimum_required(VERSION 3.1)
project(lua)

aux_source_directory(. DIR_SRCS)
list(REMOVE_ITEM DIR_SRCS lua.c src/luac.c)
add_library(${PROJECT_NAME} ${DIR_SRCS})