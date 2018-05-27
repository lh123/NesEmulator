cmake_minimum_required(VERSION 3.11)

project(imgui)

set(GLFW_USE_STATIC_LIBS TRUE)
find_package(GLFW REQUIRED)

include_directories(${GLFW_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${3rdparty_dir}/glad/include)
file(GLOB imgui_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(imgui STATIC ${imgui_src})