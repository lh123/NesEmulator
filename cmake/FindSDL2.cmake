set(SDL2_ROOT_DIR "${SDL2_ROOT_DIR}" CACHE PATH "Directory to search")

find_library(SDL2_LIBRARY
    NAMES "libSDL2.dll.a"
    PATHS ${SDL2_ROOT_DIR}
    PATH_SUFFIXES "lib")

find_library(SDL2_STATIC_LIBRARY
    NAMES "libSDL2.a"
    PATHS ${SDL2_ROOT_DIR}
    PATH_SUFFIXES "lib")

find_path(SDL2_INCLUDE_DIR
    NAMES SDL2/SDL.h
    PATHS ${SDL2_ROOT_DIR}
    PATH_SUFFIXES "include")

find_file(SDL2_RUNTIME_LIBRARY
    NAMES SDL2.dll
    PATHS ${SDL2_ROOT_DIR}
    PATH_SUFFIXES "bin")