cmake_minimum_required(VERSION 3.12)
project(volumeshOS)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

# we don't need glfw docs, tests or examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "STATIC")

# volumeshOS configuration
set(VOLUMESHOS_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(VOLUMESHOS_SRC ${VOLUMESHOS_ROOT}/src)
set(VOLUMESHOS_RES ${VOLUMESHOS_ROOT}/res)
set(VOLUMESHOS_EXT ${VOLUMESHOS_ROOT}/ext)

add_subdirectory(${VOLUMESHOS_EXT})

set(volumeshOS_SOURCES
        ${VOLUMESHOS_SRC}/main.cpp
        ${VOLUMESHOS_SRC}/ImguiRenderer.cpp
        ${VOLUMESHOS_SRC}/panels/MeshView.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/Shader.cpp
        ${VOLUMESHOS_SRC}/fs/FileManager.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/FrameBufferObject.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/VertexArrayObject.cpp
        ${VOLUMESHOS_SRC}/Window.cpp
        ${VOLUMESHOS_SRC}/panels/MenuBar.cpp
        ${VOLUMESHOS_SRC}/panels/LogWindow.cpp
        ${VOLUMESHOS_SRC}/input/Input.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshObject.cpp
        ${VOLUMESHOS_SRC}/input/Input.cpp
        ${VOLUMESHOS_SRC}/algorithms/Dijkstra.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/MeshPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/HighlightPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/BackgroundPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/ShapePass.cpp
        ${VOLUMESHOS_SRC}/rendering/shapes/Box.cpp
        ${VOLUMESHOS_SRC}/panels/CustomUIPanel.cpp
        ${VOLUMESHOS_SRC}/panels/FileDialog.cpp
        ${VOLUMESHOS_SRC}/panels/ToolBar.cpp
        ${VOLUMESHOS_SRC}/settings/GlobalViewerSettings.cpp
        ${VOLUMESHOS_SRC}/panels/FileDialog.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionPass.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/IcoSphereMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/shapes/Sphere.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/shapes/Cylinder.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionHoverPass.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/PixelBufferObject.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/ClosedCylinderMesh.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshVertexBuffer.cpp src/panels/MeshLayerView.cpp src/panels/MeshLayerView.h src/algorithms/ExampleClass.cpp src/algorithms/ExampleClass.h)

add_executable(volumeshOS ${volumeshOS_SOURCES})

target_link_libraries(volumeshOS ${CMAKE_DL_LIBS} glm glad glfw imgui OpenVolumeMesh ImGuiFileDialog stb)

# copy our resources to the build dir
if(MSVC)
    file(COPY ${VOLUMESHOS_RES} DESTINATION ${CMAKE_BINARY_DIR}/Debug)
else()
    file(COPY ${VOLUMESHOS_RES} DESTINATION ${CMAKE_BINARY_DIR})
endif(MSVC)
