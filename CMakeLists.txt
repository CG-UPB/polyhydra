cmake_minimum_required(VERSION 3.12)
project(volumeshOS)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)

if (NOT DEFINED MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()

option(VOLUMESHOS_BUILD_EXAMPLES "Build volumeshOS examples" ON)

# volumeshOS configuration
set(VOLUMESHOS_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(VOLUMESHOS_SRC ${VOLUMESHOS_ROOT}/src)
set(VOLUMESHOS_EXT ${VOLUMESHOS_ROOT}/ext)

add_subdirectory(${VOLUMESHOS_EXT})
add_subdirectory(${VOLUMESHOS_SRC})

if (VOLUMESHOS_BUILD_EXAMPLES)
    add_subdirectory(${VOLUMESHOS_ROOT}/examples)
endif ()
