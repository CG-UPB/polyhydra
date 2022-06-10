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
set(VOLUMESHOS_SRC ${VOLUMESHOS_ROOT}/volumeshOS)
set(VOLUMESHOS_RES ${VOLUMESHOS_ROOT}/res)
set(VOLUMESHOS_EXT ${VOLUMESHOS_ROOT}/ext)

add_subdirectory(${VOLUMESHOS_SRC})
add_subdirectory(${VOLUMESHOS_EXT})

if (VOLUMESHOS_BUILD_EXAMPLES)
    add_subdirectory(${VOLUMESHOS_ROOT}/examples)
endif ()

set(EXPORT_TARGET_SET)

list(APPEND EXPORT_TARGET_SET "volumeshOS_core")
list(APPEND EXPORT_TARGET_SET "glm" "glad" "glfw" "imgui" "OpenVolumeMesh" "nlohmann_json" "stb" "tinyfiledialogs")

export(TARGETS ${EXPORT_TARGET_SET} FILE "${PROJECT_BINARY_DIR}/volumeshOSTargets.cmake")
export(PACKAGE volumeshOS_core)

# copy our resources to the build dir
file(COPY ${VOLUMESHOS_RES} DESTINATION ${CMAKE_BINARY_DIR})
