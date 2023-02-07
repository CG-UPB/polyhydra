cmake_minimum_required(VERSION 3.12)
project(volumeshOS_core)

set(volumeshOS_core_SOURCES
        ${VOLUMESHOS_SRC}/Window.cpp
        ${VOLUMESHOS_SRC}/Window.h

        ${VOLUMESHOS_SRC}/fs/FileManager.cpp
        ${VOLUMESHOS_SRC}/fs/FileManager.h

        ${VOLUMESHOS_SRC}/input/Input.cpp
        ${VOLUMESHOS_SRC}/input/Input.h

        ${VOLUMESHOS_SRC}/mesh/MeshObject.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshObject.h
        ${VOLUMESHOS_SRC}/mesh/MeshVertexBuffer.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshVertexBuffer.h
        ${VOLUMESHOS_SRC}/mesh/MeshTextureBuffer.cpp
        ${VOLUMESHOS_SRC}/mesh/MeshTextureBuffer.h

        ${VOLUMESHOS_SRC}/panels/MeshView.cpp
        ${VOLUMESHOS_SRC}/panels/MeshView.h
        ${VOLUMESHOS_SRC}/panels/LogWindow.cpp
        ${VOLUMESHOS_SRC}/panels/LogWindow.h
        ${VOLUMESHOS_SRC}/panels/ToolBar.cpp
        ${VOLUMESHOS_SRC}/panels/ToolBar.h
        ${VOLUMESHOS_SRC}/panels/NewFileDialog.cpp
        ${VOLUMESHOS_SRC}/panels/NewFileDialog.h
        ${VOLUMESHOS_SRC}/panels/MeshLayerView.cpp
        ${VOLUMESHOS_SRC}/panels/MeshLayerView.h
        ${VOLUMESHOS_SRC}/panels/MeshLayerView.h
        ${VOLUMESHOS_SRC}/panels/MeshLayerView.h
        ${VOLUMESHOS_SRC}/panels/QualityPanel.cpp
        ${VOLUMESHOS_SRC}/panels/QualityPanel.h


        ${VOLUMESHOS_SRC}/rendering/Light.cpp
        ${VOLUMESHOS_SRC}/rendering/Light.h
        ${VOLUMESHOS_SRC}/rendering/Renderer.cpp
        ${VOLUMESHOS_SRC}/rendering/Renderer.h

        ${VOLUMESHOS_SRC}/rendering/camera/Camera.cpp
        ${VOLUMESHOS_SRC}/rendering/camera/Camera.h
        ${VOLUMESHOS_SRC}/rendering/camera/TrackBall.cpp
        ${VOLUMESHOS_SRC}/rendering/camera/TrackBall.h

        ${VOLUMESHOS_SRC}/rendering/gl/Shader.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/Shader.h
        ${VOLUMESHOS_SRC}/rendering/gl/FrameBufferObject.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/FrameBufferObject.h
        ${VOLUMESHOS_SRC}/rendering/gl/VertexArrayObject.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/VertexArrayObject.h
        ${VOLUMESHOS_SRC}/rendering/gl/PixelBufferObject.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/PixelBufferObject.h
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.h
        ${VOLUMESHOS_SRC}/rendering/meshes/ClosedCylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/ClosedCylinderMesh.h
        ${VOLUMESHOS_SRC}/rendering/meshes/IcoSphereMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/IcoSphereMesh.h
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.cpp
        ${VOLUMESHOS_SRC}/rendering/meshes/CylinderMesh.h
        ${VOLUMESHOS_SRC}/rendering/passes/RenderPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/MeshPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/MeshPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/BackgroundPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/BackgroundPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/VertexOnlyPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/VertexOnlyPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionHoverPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SelectionHoverPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/TransparencyPassWB.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/TransparencyPassWB.h
        ${VOLUMESHOS_SRC}/rendering/passes/TransparencyPassDP.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/TransparencyPassDP.h
        ${VOLUMESHOS_SRC}/rendering/passes/ShadowMapPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/ShadowMapPass.h
        ${VOLUMESHOS_SRC}/rendering/passes/PrePass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/PrePass.h
        ${VOLUMESHOS_SRC}/rendering/passes/SSAOPass.cpp
        ${VOLUMESHOS_SRC}/rendering/passes/SSAOPass.h
        ${VOLUMESHOS_SRC}/rendering/gl/TextureIcon.cpp
        ${VOLUMESHOS_SRC}/rendering/gl/TextureIcon.h

        ${VOLUMESHOS_SRC}/settings/AppState.cpp
        ${VOLUMESHOS_SRC}/settings/AppState.h

        ${VOLUMESHOS_SRC}/util/UIUtil.cpp
        ${VOLUMESHOS_SRC}/util/UIUtil.h
        ${VOLUMESHOS_SRC}/util/stb.cpp
        ${VOLUMESHOS_SRC}/util/Log.cpp
        ${VOLUMESHOS_SRC}/util/Log.h

        ${VOLUMESHOS_SRC}/vospch.h
        volumeshOS.h
        volumeshOS.cpp
        mesh/MeshList.cpp
        mesh/MeshList.h
        util/Types.h
        panels/WindowPanel.h
        mesh/MeshSerializer.h
        mesh/MeshSerializer.cpp
        settings/AppState.cpp
        rendering/passes/GroundPass.cpp
        rendering/passes/GroundPass.h
        rendering/shapes/ShapeRenderer.h
        rendering/shapes/ShapeRenderer.cpp rendering/shapes/Shapes.h rendering/shapes/Shapes.cpp rendering/meshes/ArrowMesh.h rendering/meshes/ArrowMesh.cpp rendering/meshes/ConeMesh.h rendering/meshes/ConeMesh.cpp rendering/passes/PostProcessingPass.h rendering/passes/PostProcessingPass.cpp rendering/passes/OutlinePass.h rendering/passes/OutlinePass.cpp)

if (MSVC)
        set(VOS_RESOURCE_BASE_PATH "..\\\\res\\\\")
else()
        set(VOS_RESOURCE_BASE_PATH "./res")
endif()

add_library(volumeshOS_core STATIC ${volumeshOS_core_SOURCES})

find_package(Filesystem REQUIRED Experimental Final)

target_precompile_headers(volumeshOS_core PUBLIC vospch.h)
target_include_directories(volumeshOS_core PUBLIC ${VOLUMESHOS_SRC})

target_link_libraries(volumeshOS_core PUBLIC ${CMAKE_DL_LIBS} glm glad glfw imgui OpenVolumeMesh nlohmann_json::nlohmann_json stb std::filesystem tinyfiledialogs)

target_compile_definitions(volumeshOS_core PUBLIC FS_HEADER=<${CXX_FILESYSTEM_HEADER}> FS_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE} VOS_RESOURCE_BASE_PATH="${VOS_RESOURCE_BASE_PATH}")
