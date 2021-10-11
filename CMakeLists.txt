cmake_minimum_required(VERSION 3.12)
project(volumeshOS)

set(CMAKE_CXX_STANDARD 14)

# we don't need glfw docs, tests or examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# volumeshOS configuration
set(VOLUMESHOS_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(VOLUMESHOS_SRC ${VOLUMESHOS_ROOT}/src)
set(VOLUMESHOS_RES ${VOLUMESHOS_ROOT}/res)
set(VOLUMESHOS_EXT ${VOLUMESHOS_ROOT}/ext)

add_subdirectory(ext)

set(volumeshOS_SOURCES
        src/main.cpp
        src/Window.h
        src/Window.cpp
        src/panels/MeshView.cpp
        src/panels/MeshView.h
        src/rendering/Shader.cpp
        src/rendering/Shader.h
        src/fs/FileManager.cpp
        src/fs/FileManager.h
        src/rendering/FrameBufferObject.cpp
        src/rendering/FrameBufferObject.h
        src/rendering/VertexArrayObject.cpp
        src/rendering/VertexArrayObject.h)

add_executable(volumeshOS ${volumeshOS_SOURCES})

target_link_libraries(volumeshOS glm glew_s glfw imgui OpenVolumeMesh ${CMAKE_DL_LIBS})

# copy our resources to the build dir
file(COPY ${VOLUMESHOS_RES} DESTINATION ${CMAKE_BINARY_DIR})
