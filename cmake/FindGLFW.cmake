set(GLFW_ROOT_DIR "${GLFW_ROOT_DIR}" CACHE PATH "Directory to search")

find_library(GLFW_LIBRARY
    NAMES "libglfw3.dll.a" "libglfw3.dll" "glfw3.dll"
    PATHS ${GLFW_ROOT_DIR}
    PATH_SUFFIXES "lib-mingw-w64")

find_library(GLFW_STATIC_LIBRARY
    NAMES "libglfw3.a"
    PATHS ${GLFW_ROOT_DIR}
    PATH_SUFFIXES "lib-mingw-w64")

find_path(GLFW_INCLUDE_DIR
    NAMES GLFW/glfw3.h
    PATHS ${GLFW_ROOT_DIR}
    PATH_SUFFIXES "include")

find_file(GLFW_RUNTIME_LIBRARY
    NAMES "glfw3.dll"
    PATHS ${GLFW_ROOT_DIR}
    PATH_SUFFIXES "lib-mingw-w64")