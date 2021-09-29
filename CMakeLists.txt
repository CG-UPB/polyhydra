cmake_minimum_required(VERSION 3.20)
project(volumeshOS)

set(CMAKE_CXX_STANDARD 14)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(ext/glfw)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

file(GLOB Main_Sources "src/*.cpp")
file(GLOB ImGUI_Sources "ext/imgui/*.cpp")

add_executable(volumeshOS ${Main_Sources} ${ImGUI_Sources})

target_include_directories(volumeshOS PUBLIC ext/imgui)
target_include_directories(volumeshOS PUBLIC ext/imgui/backends)
target_link_libraries(volumeshOS OpenVolumeMesh glfw ${OPENGL_LIBRARIES})
