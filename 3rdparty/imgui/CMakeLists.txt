cmake_minimum_required(VERSION 3.11)

project(imgui)

find_package(GLFW)

if(GLFW_INCLUDE_DIR)
    include_directories(${GLFW_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find GLFW")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${3rdparty_dir}/glad/include)
file(GLOB imgui_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(imgui STATIC ${imgui_src})