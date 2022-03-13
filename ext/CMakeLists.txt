cmake_minimum_required(VERSION 3.12)


###########################
# Git Submodules
###########################

# update all submodules before building
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # Update submodules as needed
    message(STATUS "Submodule init")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -f
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/OpenVolumeMesh/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glm/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/json-develop/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image_write.h"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tinyfiledialogs/tinyfiledialogs.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


###########################
# CMake Dependencies
###########################

# all regular cmake dependencies
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(OpenVolumeMesh)



###########################
# ImGui
###########################

# imgui is not built with cmake by default, so we need to include the files ourselves
project(imgui)

set(imgui_HEADERS
        imgui/imgui.h
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.h
        )

set(imgui_SOURCES
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        )

add_library(imgui STATIC ${imgui_SOURCES} ${imgui_HEADERS})

set_target_properties(imgui PROPERTIES FOLDER "ext")

set(IMGUI_INCLUDE_DIR  ${CMAKE_CURRENT_LIST_DIR}/imgui ${CMAKE_CURRENT_LIST_DIR}/imgui/backends ${VOLUMESHOS_EXT}/glad/include)
target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR})

target_link_libraries(imgui PUBLIC glad glfw)

###########################
# stb
###########################

# add stb library
set(stb_DIR stb)
set(stb_Headers
        ${stb_DIR}/stb_c_lexer.h
        ${stb_DIR}/stb_connected_components.h
        ${stb_DIR}/stb_divide.h
        ${stb_DIR}/stb_ds.h
        ${stb_DIR}/stb_dxt.h
        ${stb_DIR}/stb_easy_font.h
        ${stb_DIR}/stb_herringbone_wang_tile.h
        ${stb_DIR}/stb_hexwave.h
        ${stb_DIR}/stb_image.h
        ${stb_DIR}/stb_image_resize.h
        ${stb_DIR}/stb_image_write.h
        ${stb_DIR}/stb_include.h
        ${stb_DIR}/stb_leakcheck.h
        ${stb_DIR}/stb_rect_pack.h
        ${stb_DIR}/stb_sprintf.h
        ${stb_DIR}/stb_textedit.h
        ${stb_DIR}/stb_tilemap_editor.h
        ${stb_DIR}/stb_truetype.h
        ${stb_DIR}/stb_voxel_render.h)

set(stb_Sources ${stb_DIR}/stb_vorbis.c)

add_library(stb STATIC ${stb_Headers} ${stb_Sources})
target_include_directories(stb PUBLIC ${stb_DIR})

###########################
# tinyfiledialogs
###########################

# add tinyfiledialogs library
set(tinyfiledialogs_DIR tinyfiledialogs)
set(tinyfiledialogs_Headers ${tinyfiledialogs_DIR}/tinyfiledialogs.h)
set(tinyfiledialogs_Sources ${tinyfiledialogs_DIR}/tinyfiledialogs.c)

add_library(tinyfiledialogs STATIC ${tinyfiledialogs_Headers} ${tinyfiledialogs_Sources})
target_include_directories(tinyfiledialogs PUBLIC ${tinyfiledialogs_DIR})