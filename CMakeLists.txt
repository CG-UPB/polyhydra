cmake_minimum_required(VERSION 3.12)

project(polyhydra VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build this in a source directory (or any directory with a CMakeLists.txt file). "
                        "Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

### Build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  "Debug" "Release" "RelWithDebInfo")
endif()

### Standalone?
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME}) # can use PROJECT_IS_TOP_LEVEL in cmake 3.21
    set(POLYHYDRA_STANDALONE TRUE)
    message("Configuring polyhydra as standalone project...")
else()
    set(POLYHYDRA_STANDALONE FALSE)
    message("Configuring polyhydra inside another cmake project...")
endif()

### Options
option(POLYHYDRA_BUILD_EXAMPLES "Build polyhydra examples" ${POLYHYDRA_STANDALONE})
option(POLYHYDRA_SUBMODULES_MANUAL "Skip automatic submodule download" OFF)

if (POLYHYDRA_STANDALONE)
    ### Global cmake settings
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    if (NOT DEFINED MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    endif ()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Build/lib/)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Build/lib/)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Build/bin/)
endif()

# polyhydra configuration
set(POLYHYDRA_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(POLYHYDRA_SRC ${POLYHYDRA_ROOT}/src)
set(POLYHYDRA_INC ${POLYHYDRA_ROOT}/include)
set(POLYHYDRA_EXT ${POLYHYDRA_ROOT}/ext)

if (NOT POLYHYDRA_SUBMODULES_MANUAL)
    add_subdirectory(${POLYHYDRA_EXT})
endif()

add_subdirectory(${POLYHYDRA_SRC})

if (POLYHYDRA_BUILD_EXAMPLES)
    add_subdirectory(${POLYHYDRA_ROOT}/examples)
endif ()

### Fake successful finder run if compiling as a dependent project.
if (NOT POLYHYDRA_STANDALONE)
    set(polyhydra_FOUND true PARENT_SCOPE)
    set(polyhydra_LIBRARIES polyhydra PARENT_SCOPE)
    set(polyhydra_LIBRARY polyhydra PARENT_SCOPE)
    set(polyhydra_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/" PARENT_SCOPE)
endif()
