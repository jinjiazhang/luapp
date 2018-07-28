cmake_minimum_required(VERSION 3.1)
project(lualib)

aux_source_directory(. DIR_SRCS)
add_library(${PROJECT_NAME} ${DIR_SRCS})

include_directories(../)
include_directories(../include)