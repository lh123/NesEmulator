set(GLFW_ROOT_DIR ${GLFW_ROOT_DIR} CACHE PATH "Directory to search")

if(NOT DEFINED GLFW_USE_STATIC_LIBS)
    set(GLFW_USE_STATIC_LIBS ON 
    CACHE BOOL 
    "Set to ON to force the use of the static")
endif()

if(GLFW_USE_STATIC_LIBS)
    set(GLFW_LIB_NAME "libglfw3.a")
else()
    set(GLFW_LIB_NAME "libglfw3dll.a")
endif()

find_path(GLFW_INCLUDE_DIR
    NAMES GLFW/glfw3.h
    PATHS "${GLFW_ROOT_DIR}/include")

find_library(GLFW_LIBRARY
    NAMES ${GLFW_LIB_NAME}
    PATHS "${GLFW_ROOT_DIR}/lib-mingw-w64")

if(NOT GLFW_USE_STATIC_LIBS)
    find_file(GLFW_RUNTIME_LIBRARY
        NAMES "glfw3.dll"
        PATHS "${GLFW_ROOT_DIR}/lib-mingw-w64")
endif()

if(GLFW_INCLUDE_DIR AND GLFW_LIBRARY)
    set(GLFW_FOUND ON)
else()
    set(GLFW_FOUND OFF)
endif()

if(NOT GLFW_USE_STATIC_LIBS AND NOT GLFW_RUNTIME_LIBRARY)
    set(GLFW_FOUND OFF)
endif()