cmake_minimum_required(VERSION 3.11)
project(NesEmulator)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-static -mwindows ${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-Map,NesEmulator.map")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(3rdparty_dir ${PROJECT_SOURCE_DIR}/3rdparty)

add_subdirectory(${3rdparty_dir}/glad)
add_subdirectory(${3rdparty_dir}/imgui)

set(GLFW_USE_STATIC_LIBS ON)
find_package(GLFW REQUIRED)

set(SDL2_USE_STATIC_LIBS ON)
find_package(SDL2 REQUIRED)

include_directories(${GLFW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include
    ${3rdparty_dir}/glad/include
    ${3rdparty_dir}/imgui/include)

file(GLOB_RECURSE src_list ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(NesEmulator ${src_list})

set(link_libraries imgui ${GLFW_LIBRARIES} glad ${SDL2_LIBRARIES} ws2_32 comdlg32 opengl32 gdi32)

target_link_libraries(NesEmulator ${link_libraries})

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