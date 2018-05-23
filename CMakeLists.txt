cmake_minimum_required(VERSION 3.11)
project(NesEmulator)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-static -mwindows ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(3rdparty_dir ${PROJECT_SOURCE_DIR}/3rdparty)

add_subdirectory(${3rdparty_dir}/glad)
add_subdirectory(${3rdparty_dir}/imgui)

set(GLFW_USE_STATIC_LIBS TRUE)
find_package(GLFW)

if(GLFW_FOUND)
    include_directories(${GLFW_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find GLFW")
endif()

set(SDL2_USE_STATIC_LIBS TRUE)
find_package(SDL2)

if(SDL2_FOUND)
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
    imgui ${GLFW_LIBRARY} glad ${SDL2_LIBRARY} ws2_32 comdlg32 opengl32 gdi32)

if(SDL2_USE_STATIC_LIBS)
    target_link_libraries(NesEmulator
        ${SDL2_LINK_LIBRARY})
endif()

install(TARGETS NesEmulator
    RUNTIME DESTINATION ".")

if(NOT GLFW_USE_STATIC_LIBS)
    install(FILES ${GLFW_RUNTIME_LIBRARY}
        DESTINATION ".")
endif()

if(NOT SDL2_USE_STATIC_LIBS)
    install(FILES ${SDL2_RUNTIME_LIBRARY}
        DESTINATION ".")
endif()

message(STATUS "Current Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Current C++ Complier Flag:")
message(STATUS "    Release:    ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "    Debug:      ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "")
message(STATUS "Current Link Flag:")
message(STATUS "    Release:    ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message(STATUS "    Debug:      ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
message(STATUS "")
message(STATUS "Install to:     ${CMAKE_INSTALL_PREFIX}")