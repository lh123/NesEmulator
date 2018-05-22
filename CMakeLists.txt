cmake_minimum_required(VERSION 3.11)
project(NesEmulator)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "-Wall -O2")
    set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -mwindows")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(3rdparty_dir ${PROJECT_SOURCE_DIR}/3rdparty)

add_subdirectory(${3rdparty_dir}/glad)
add_subdirectory(${3rdparty_dir}/imgui)

find_package(GLFW)

if(GLFW_INCLUDE_DIR)
    include_directories(${GLFW_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find GLFW")
endif()

find_package(SDL2)

if(SDL2_INCLUDE_DIR)
    include_directories(${SDL2_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find SDL2")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include
    ${3rdparty_dir}/glad/include
    ${3rdparty_dir}/imgui/include)

file(GLOB_RECURSE src_list ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(NesEmulator ${src_list})

target_link_libraries(NesEmulator
    imgui ${GLFW_STATIC_LIBRARY} glad ${SDL2_LIBRARY} ws2_32 comdlg32 opengl32 gdi32)

install(TARGETS NesEmulator
    RUNTIME DESTINATION ".")

install(FILES ${SDL2_RUNTIME_LIBRARY}
    DESTINATION ".")

message(STATUS "Current Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Current C++ Complier Flag: ${CMAKE_CXX_FLAGS}")
message(STATUS "Current Link Flag: ${CMAKE_LINK_FLAGS}")