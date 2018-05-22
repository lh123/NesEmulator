cmake_minimum_required(VERSION 3.11)

project(glad)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB glad_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(glad STATIC ${glad_src})