cmake_minimum_required(VERSION 3.12)
project(volumeshOS)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)

if(NOT DEFINED MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif(NOT DEFINED MSVC)

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
        ${VOLUMESHOS_SRC}/panels/LogWindow.cpp
        ${VOLUMESHOS_SRC}/input/Input.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshObject.cpp
        ${VOLUMESHOS_SRC}/input/Input.cpp
        ${VOLUMESHOS_SRC}/algorithms/Dijkstra.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/MeshPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/BackgroundPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/ShapePass.cpp
        ${VOLUMESHOS_SRC}/rendering/shapes/Box.cpp
        ${VOLUMESHOS_SRC}/panels/CustomUIPanel.cpp
        ${VOLUMESHOS_SRC}/panels/ToolBar.cpp
        ${VOLUMESHOS_SRC}/settings/GlobalViewerSettings.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionPass.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/IcoSphereMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/shapes/Sphere.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/shapes/Cylinder.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionHoverPass.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/PixelBufferObject.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/ClosedCylinderMesh.cpp
        ${VOLUMESHOS_SRC}/panels/NewFileDialog.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshVertexBuffer.cpp
        ${VOLUMESHOS_SRC}/panels/MeshLayerView.cpp
        ${VOLUMESHOS_SRC}/panels/MeshLayerView.h
        ${VOLUMESHOS_SRC}/algorithms/ExampleClass.cpp
        ${VOLUMESHOS_SRC}/algorithms/ExampleClass.h
        ${VOLUMESHOS_SRC}/rendering/passes/TransparencyPass_WB.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/TransparencyPass_DP.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/ShadowMapPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/TransparentShadowMapPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/ShadowColorFilterPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/PrePass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SSAOPass.cpp
        ${VOLUMESHOS_SRC}/algorithms/TestClass.cpp
        ${VOLUMESHOS_SRC}/algorithms/TestQuads.cpp
        ${VOLUMESHOS_SRC}/rendering/Camera.cpp
        ${VOLUMESHOS_SRC}/rendering/Light.cpp
        ${VOLUMESHOS_SRC}/panels/QualityPanel.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/VertexOnlyPass.cpp
        ${VOLUMESHOS_SRC}/rendering/MeshMover.cpp
        ${VOLUMESHOS_SRC}/algorithms/Demo.cpp
        ${VOLUMESHOS_SRC}/rendering/Renderer.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/TextureIcon.cpp
        ${VOLUMESHOS_SRC}/util/UIUtil.cpp
        ${VOLUMESHOS_SRC}/util/stb.cpp
        ${VOLUMESHOS_SRC}/util/Log.cpp)

add_executable(volumeshOS ${volumeshOS_SOURCES})

#json
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(ext/json-develop)

find_package(Filesystem REQUIRED Experimental Final)

target_precompile_headers(volumeshOS PRIVATE ${VOLUMESHOS_SRC}/vospch.h)
target_include_directories(volumeshOS PRIVATE ${VOLUMESHOS_SRC})

target_link_libraries(volumeshOS ${CMAKE_DL_LIBS} glm glad glfw imgui OpenVolumeMesh nlohmann_json::nlohmann_json stb std::filesystem tinyfiledialogs)
target_compile_definitions(volumeshOS PUBLIC FS_HEADER=<${CXX_FILESYSTEM_HEADER}> FS_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE})

# copy our resources to the build dir
file(COPY ${VOLUMESHOS_RES} DESTINATION ${CMAKE_BINARY_DIR})
