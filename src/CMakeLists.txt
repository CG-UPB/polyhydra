set(volumeshOS_SOURCES
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
        rendering/shapes/ShapeRenderer.cpp rendering/shapes/Shapes.h rendering/shapes/Shapes.cpp rendering/meshes/ArrowMesh.h rendering/meshes/ArrowMesh.cpp rendering/meshes/ConeMesh.h rendering/meshes/ConeMesh.cpp rendering/passes/PostProcessingPass.h rendering/passes/PostProcessingPass.cpp rendering/passes/OutlinePass.h rendering/passes/OutlinePass.cpp util/RingBuffer.cpp)

if (MSVC)
        set(VOS_RESOURCE_BASE_PATH "..\\\\res\\\\")
else()
        set(VOS_RESOURCE_BASE_PATH "./res")
endif()

add_library(volumeshOS STATIC ${volumeshOS_SOURCES})
add_library(volumeshOS::volumeshOS ALIAS volumeshOS)

### Configure install paths
include(CMakePackageConfigHelpers)
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR "CMake")
else()
  set(DEF_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/MC3D")
endif()
set(volumeshOS_INSTALL_CMAKE_DIR "${DEF_INSTALL_CMAKE_DIR}" CACHE PATH "Installation directory for CMake files")
set(volumeshOS_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Installation directory for libraries")
set(volumeshOS_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Installation directory for header files")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(CONF_LIB_DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
configure_package_config_file(../volumeshOSConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/volumeshOSConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
# ... for the install tree
set(CONF_INCLUDE_DIRS "${volumeshOS_INSTALL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "${volumeshOS_INSTALL_LIB_DIR}")
configure_package_config_file(../volumeshOSConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/volumeshOSConfig.cmake"
  INSTALL_DESTINATION "${volumeshOS_INSTALL_CMAKE_DIR}")

target_include_directories(volumeshOS
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
  "$<INSTALL_INTERFACE:${volumeshOS_INSTALL_INCLUDE_DIR}>")

find_package(Filesystem REQUIRED Experimental Final)

target_link_libraries(volumeshOS PUBLIC ${CMAKE_DL_LIBS} glm glad glfw imgui OpenVolumeMesh nlohmann_json::nlohmann_json stb std::filesystem tinyfiledialogs)

set_target_properties(volumeshOS PROPERTIES
                      DEBUG_POSTFIX "d"
                      VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
                      SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON)

target_precompile_headers(volumeshOS PUBLIC vospch.h)

target_compile_definitions(volumeshOS PRIVATE FS_HEADER=<${CXX_FILESYSTEM_HEADER}> FS_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE})
target_compile_definitions(volumeshOS PUBLIC VOS_RESOURCE_BASE_PATH="${VOS_RESOURCE_BASE_PATH}")

# copy our resources to the build dir
set(VOLUMESHOS_RES ${VOLUMESHOS_ROOT}/res)
file(COPY ${VOLUMESHOS_RES} DESTINATION ${CMAKE_BINARY_DIR})

install(DIRECTORY
        "${VOLUMESHOS_RES}"
        DESTINATION ${CMAKE_INSTALL_BINDIR})

### Install and export
install(DIRECTORY "${VOLUMESHOS_SRC}"
        DESTINATION "${volumeshOS_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/volumeshOSConfig.cmake"
        DESTINATION "${volumeshOS_INSTALL_INCLUDE_DIR}")

install(TARGETS       volumeshOS
        EXPORT        volumeshOSTargets
        LIBRARY       DESTINATION "${volumeshOS_INSTALL_LIB_DIR}"
        ARCHIVE       DESTINATION "${volumeshOS_INSTALL_LIB_DIR}")

install(EXPORT volumeshOSTargets
        FILE volumeshOSTargets.cmake
        NAMESPACE volumeshOS::
        DESTINATION "${volumeshOS_INSTALL_CMAKE_DIR}")

export(EXPORT volumeshOSTargets
       NAMESPACE volumeshOS::)

set(VOLUMESHOS_RES ${VOLUMESHOS_ROOT}/res)
