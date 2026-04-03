cmake_minimum_required(VERSION 3.12)
project(polyhydra)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)

if (NOT DEFINED MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()

option(POLYHYDRA_BUILD_EXAMPLES "Build polyhydra examples" ON)

# polyhydra configuration
set(POLYHYDRA_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(POLYHYDRA_SRC ${POLYHYDRA_ROOT}/src)
set(POLYHYDRA_INC ${POLYHYDRA_ROOT}/include)
set(POLYHYDRA_EXT ${POLYHYDRA_ROOT}/ext)

add_subdirectory(${POLYHYDRA_EXT})
add_subdirectory(${POLYHYDRA_SRC})

if (POLYHYDRA_BUILD_EXAMPLES)
    add_subdirectory(${POLYHYDRA_ROOT}/examples)
endif ()
