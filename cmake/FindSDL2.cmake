set(SDL2_ROOT_DIR ${SDL2_ROOT_DIR} CACHE PATH "Directory to search")

if(NOT DEFINED SDL2_USE_STATIC_LIBS)
    set(SDL2_USE_STATIC_LIBS TRUE 
    CACHE BOOL 
    "Set to ON to force the use of the static")
endif()

if(SDL2_USE_STATIC_LIBS)
    set(SDL2_LIB_NAME "libSDL2.a")
    set(SDL2_LINK_LIBRARY "Imm32" "Version" "Winmm")
else()
    set(SDL2_LIB_NAME "libSDL2.dll.a")
endif()

find_path(SDL2_INCLUDE_DIR
    NAMES SDL2/SDL.h
    PATHS "${SDL2_ROOT_DIR}/include")

find_library(SDL2_LIBRARY
    NAMES ${SDL2_LIB_NAME}
    PATHS "${SDL2_ROOT_DIR}/lib")

if(NOT SDL2_USE_STATIC_LIBS)
    find_file(SDL2_RUNTIME_LIBRARY
        NAMES "SDL2.dll"
        PATHS "${SDL2_ROOT_DIR}/bin")
endif()

if(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
    set(SDL2_FOUND TRUE)
else()
    set(SDL2_FOUND FALSE)
endif()

if(NOT SDL2_USE_STATIC_LIBS AND NOT SDL2_RUNTIME_LIBRARY)
    set(SDL2_FOUND FALSE)
endif()