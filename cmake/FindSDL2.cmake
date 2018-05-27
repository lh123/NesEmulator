set(SDL2_ROOT_DIR ${SDL2_ROOT_DIR} CACHE PATH "Directory to search")

if(NOT DEFINED SDL2_USE_STATIC_LIBS)
    set(SDL2_USE_STATIC_LIBS TRUE 
    CACHE BOOL 
    "Set to ON to force the use of the static")
endif()

if(SDL2_USE_STATIC_LIBS)
    set(SDL2_LIB_NAME "libSDL2.a")
    set(SDL2_ADDITION_LINK_LIBRARIES "Imm32" "Version" "Winmm")
else()
    set(SDL2_LIB_NAME "libSDL2.dll.a")
    set(SDL2_ADDITION_LINK_LIBRARIES "")
endif()

find_path(SDL2_INCLUDE_DIRS
    NAMES SDL2/SDL.h
    PATHS "${SDL2_ROOT_DIR}/include")

find_library(SDL2_LIBRARIES
    NAMES ${SDL2_LIB_NAME}
    PATHS "${SDL2_ROOT_DIR}/lib")

if(SDL2_LIBRARIES)
    list(APPEND SDL2_LIBRARIES ${SDL2_ADDITION_LINK_LIBRARIES})
endif()

if(NOT SDL2_USE_STATIC_LIBS)
    find_file(SDL2_RUNTIME_LIBRARY
        NAMES "SDL2.dll"
        PATHS "${SDL2_ROOT_DIR}/bin")
endif()

if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARIES)
    set(SDL2_FOUND TRUE)
else()
    set(SDL2_FOUND FALSE)
endif()

if(NOT SDL2_USE_STATIC_LIBS AND NOT SDL2_RUNTIME_LIBRARY)
    set(SDL2_FOUND FALSE)
endif()

if(NOT SDL2_FOUND AND SDL2_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find SDL2")
endif()