cmake_minimum_required(VERSION 3.1)
project(lualib)

aux_source_directory(. DIR_SRCS)
file(GLOB_RECURSE DIR_INCS *.h *.hpp)
source_group("Include Files" FILES ${DIR_INCS}) 

add_library(${PROJECT_NAME} ${DIR_SRCS} ${DIR_INCS})

include_directories(../)
include_directories(../include)