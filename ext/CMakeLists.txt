cmake_minimum_required(VERSION 3.12)

# all regular cmake dependencies
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(OpenVolumeMesh)

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