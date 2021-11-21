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

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.h"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/OpenVolumeMesh/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glm/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h")
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
# ImGuiFileDialog
###########################

# add ImGuiFileDialog library
set(ImGuiFileDialog_DIR ImGuiFileDialog )
set(ImGuiFileDialog_Headers ${ImGuiFileDialog_DIR}/ImGuiFileDialog.h)
set(ImGuiFileDialog_Sources ${ImGuiFileDialog_DIR}/ImGuiFileDialog.cpp)

add_library(ImGuiFileDialog ${ImGuiFileDialog_Headers} ${ImGuiFileDialog_Sources})
target_include_directories(ImGuiFileDialog PUBLIC ${ImGuiFileDialog_DIR})

target_link_libraries(ImGuiFileDialog PUBLIC imgui)