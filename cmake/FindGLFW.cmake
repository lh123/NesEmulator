set(GLFW_ROOT_DIR ${GLFW_ROOT_DIR} CACHE PATH "Directory to search")

if(NOT DEFINED GLFW_USE_STATIC_LIBS)
    set(GLFW_USE_STATIC_LIBS TRUE 
    CACHE BOOL 
    "Set to TRUE to force the use of the static")
endif()

if(GLFW_USE_STATIC_LIBS)
    set(GLFW_LIB_NAME "libglfw3.a")
else()
    set(GLFW_LIB_NAME "libglfw3dll.a")
endif()

find_path(GLFW_INCLUDE_DIRS
    NAMES GLFW/glfw3.h
    PATHS "${GLFW_ROOT_DIR}/include")

find_library(GLFW_LIBRARIES
    NAMES ${GLFW_LIB_NAME}
    PATHS "${GLFW_ROOT_DIR}/lib-mingw-w64")

if(NOT GLFW_USE_STATIC_LIBS)
    find_file(GLFW_RUNTIME_LIBRARIES
        NAMES "glfw3.dll"
        PATHS "${GLFW_ROOT_DIR}/lib-mingw-w64")
endif()

if(GLFW_INCLUDE_DIRS AND GLFW_LIBRARIES)
    set(GLFW_FOUND TRUE)
else()
    set(GLFW_FOUND FALSE)
endif()

if(NOT GLFW_USE_STATIC_LIBS AND NOT GLFW_RUNTIME_LIBRARIES)
    set(GLFW_FOUND FALSE)
endif()

if(NOT GLFW_FOUND AND GLFW_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find GLFW")
endif()