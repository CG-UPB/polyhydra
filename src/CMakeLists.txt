macro(clean_interface_include_directories target)
    get_target_property(TARGET_INCLUDE_DIRS ${target} INTERFACE_INCLUDE_DIRECTORIES)

    if(TARGET_INCLUDE_DIRS)
        # Create a new list to hold the modified include directories
        set(MODIFIED_INCLUDE_DIRS)

        # Iterate over each include directory in the list
        foreach(INCLUDE_DIR ${TARGET_INCLUDE_DIRS})
            # Check if the directory path is relative to the source directory
            if(NOT INCLUDE_DIR STREQUAL ${CMAKE_SOURCE_DIR}
               AND (NOT INCLUDE_DIR MATCHES "^BUILD_INTERFACE")
               AND INCLUDE_DIR MATCHES "^${CMAKE_SOURCE_DIR}")
                # If it's relative to the source, wrap it in BUILD_INTERFACE
                list(APPEND MODIFIED_INCLUDE_DIRS "$<BUILD_INTERFACE:${INCLUDE_DIR}>")
                list(APPEND MODIFIED_INCLUDE_DIRS "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>")
            else()
                # If it's not relative to the source directory, keep it as is
                list(APPEND MODIFIED_INCLUDE_DIRS "${INCLUDE_DIR}")
            endif()
        endforeach()

        # Set the modified include directories back to the target's property
        set_target_properties(${target} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MODIFIED_INCLUDE_DIRS}")
    else()
        message(WARNING "No INTERFACE_INCLUDE_DIRECTORIES found for target ${target}")
    endif()
endmacro()

###########################
# CMake Dependencies
###########################

# all regular cmake dependencies
add_subdirectory(${VOLUMESHOS_EXT}/glad ext/glad EXCLUDE_FROM_ALL)
clean_interface_include_directories(glad)

# we don't need glfw docs, tests or examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "STATIC")

add_subdirectory(${VOLUMESHOS_EXT}/glfw ext/glfw EXCLUDE_FROM_ALL)
clean_interface_include_directories(glfw)

add_subdirectory(${VOLUMESHOS_EXT}/glm ext/glm EXCLUDE_FROM_ALL)
clean_interface_include_directories(glm)
if (NOT TARGET OpenVolumeMesh::OpenVolumeMesh)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/OpenVolumeMesh/CMakeLists.txt")
        add_subdirectory(${VOLUMESHOS_EXT}/OpenVolumeMesh OpenVolumeMesh EXCLUDE_FROM_ALL)
    else()
        find_package(OpenVolumeMesh REQUIRED)
    endif()
endif()

#json
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(${VOLUMESHOS_EXT}/json-develop ext/json-develop EXCLUDE_FROM_ALL)

###########################
# ImGui
###########################

# imgui is not built with cmake by default, so we need to include the files ourselves
set(imgui_HEADERS
        ${VOLUMESHOS_EXT}/imgui/imgui.h
        ${VOLUMESHOS_EXT}/imgui/backends/imgui_impl_glfw.h
        ${VOLUMESHOS_EXT}/imgui/backends/imgui_impl_opengl3.h
        ${VOLUMESHOS_EXT}/imgui/misc/cpp/imgui_stdlib.h
        )

set(imgui_SOURCES
        ${VOLUMESHOS_EXT}/imgui/imgui.cpp
        ${VOLUMESHOS_EXT}/imgui/imgui_demo.cpp
        ${VOLUMESHOS_EXT}/imgui/imgui_draw.cpp
        ${VOLUMESHOS_EXT}/imgui/imgui_widgets.cpp
        ${VOLUMESHOS_EXT}/imgui/imgui_tables.cpp
        ${VOLUMESHOS_EXT}/imgui/backends/imgui_impl_glfw.cpp
        ${VOLUMESHOS_EXT}/imgui/backends/imgui_impl_opengl3.cpp
        ${VOLUMESHOS_EXT}/imgui/misc/cpp/imgui_stdlib.cpp
        )

add_library(imgui STATIC ${imgui_SOURCES} ${imgui_HEADERS})

set_target_properties(imgui PROPERTIES FOLDER "ext")

set(IMGUI_INCLUDE_DIR  ${VOLUMESHOS_EXT}/imgui ${VOLUMESHOS_EXT}/imgui/backends ${VOLUMESHOS_EXT}/glad/include)
target_include_directories(imgui
                           PUBLIC
                           "$<BUILD_INTERFACE:${VOLUMESHOS_EXT}/imgui>"
                           "$<BUILD_INTERFACE:${VOLUMESHOS_EXT}/imgui/backends>"
                           "$<BUILD_INTERFACE:${VOLUMESHOS_EXT}/glad/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>")

target_link_libraries(imgui PRIVATE glad glfw)
if (MINGW)
    target_link_libraries(imgui PRIVATE imm32)
endif()

###########################
# stb
###########################

# add stb library
set(stb_DIR ${VOLUMESHOS_EXT}/stb)
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
target_include_directories(stb PUBLIC
                           "$<BUILD_INTERFACE:${stb_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>")

###########################
# tinyfiledialogs
###########################

# add tinyfiledialogs library
set(tinyfiledialogs_DIR ${VOLUMESHOS_EXT}/tinyfiledialogs)
set(tinyfiledialogs_Headers ${tinyfiledialogs_DIR}/tinyfiledialogs.h)
set(tinyfiledialogs_Sources ${tinyfiledialogs_DIR}/tinyfiledialogs.c)

add_library(tinyfiledialogs STATIC ${tinyfiledialogs_Headers} ${tinyfiledialogs_Sources})
target_include_directories(tinyfiledialogs PUBLIC
                           "$<BUILD_INTERFACE:${tinyfiledialogs_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>")


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

target_link_libraries(volumeshOS PUBLIC ${CMAKE_DL_LIBS} imgui OpenVolumeMesh::OpenVolumeMesh glm glad glfw nlohmann_json::nlohmann_json stb std::filesystem tinyfiledialogs)

set_target_properties(volumeshOS PROPERTIES
                      DEBUG_POSTFIX "d"
                      VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
                      SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON)

target_precompile_headers(volumeshOS PUBLIC vospch.h)

target_compile_definitions(volumeshOS PUBLIC VOS_RESOURCE_BASE_PATH="${VOS_RESOURCE_BASE_PATH}" FS_HEADER=<${CXX_FILESYSTEM_HEADER}> FS_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE})
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

install(TARGETS       volumeshOS glm glfw glad imgui nlohmann_json stb tinyfiledialogs
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
