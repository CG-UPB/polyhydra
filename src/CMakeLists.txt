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
add_subdirectory(${POLYHYDRA_EXT}/glad ext/glad EXCLUDE_FROM_ALL)
clean_interface_include_directories(glad)

# we don't need glfw docs, tests or examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "STATIC")

add_subdirectory(${POLYHYDRA_EXT}/glfw ext/glfw EXCLUDE_FROM_ALL)
clean_interface_include_directories(glfw)

add_subdirectory(${POLYHYDRA_EXT}/glm ext/glm EXCLUDE_FROM_ALL)
clean_interface_include_directories(glm)
if (NOT TARGET OpenVolumeMesh::OpenVolumeMesh)
    if(EXISTS "${POLYHYDRA_EXT}/OpenVolumeMesh/CMakeLists.txt")
        add_subdirectory(${POLYHYDRA_EXT}/OpenVolumeMesh OpenVolumeMesh EXCLUDE_FROM_ALL)
    else()
        find_package(OpenVolumeMesh REQUIRED)
    endif()
endif()

#json
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(${POLYHYDRA_EXT}/json-develop ext/json-develop EXCLUDE_FROM_ALL)

###########################
# ImGui
###########################

# imgui is not built with cmake by default, so we need to include the files ourselves
set(imgui_HEADERS
        ${POLYHYDRA_EXT}/imgui/imgui.h
        ${POLYHYDRA_EXT}/imgui/backends/imgui_impl_glfw.h
        ${POLYHYDRA_EXT}/imgui/backends/imgui_impl_opengl3.h
        ${POLYHYDRA_EXT}/imgui/misc/cpp/imgui_stdlib.h
        )

set(imgui_SOURCES
        ${POLYHYDRA_EXT}/imgui/imgui.cpp
        ${POLYHYDRA_EXT}/imgui/imgui_demo.cpp
        ${POLYHYDRA_EXT}/imgui/imgui_draw.cpp
        ${POLYHYDRA_EXT}/imgui/imgui_widgets.cpp
        ${POLYHYDRA_EXT}/imgui/imgui_tables.cpp
        ${POLYHYDRA_EXT}/imgui/backends/imgui_impl_glfw.cpp
        ${POLYHYDRA_EXT}/imgui/backends/imgui_impl_opengl3.cpp
        ${POLYHYDRA_EXT}/imgui/misc/cpp/imgui_stdlib.cpp
        )

add_library(imgui STATIC ${imgui_SOURCES} ${imgui_HEADERS})

set_target_properties(imgui PROPERTIES FOLDER "ext")

set(IMGUI_INCLUDE_DIR  ${POLYHYDRA_EXT}/imgui ${POLYHYDRA_EXT}/imgui/backends ${POLYHYDRA_EXT}/glad/include)
target_include_directories(imgui
                           PUBLIC
                           "$<BUILD_INTERFACE:${POLYHYDRA_EXT}/imgui>"
                           "$<BUILD_INTERFACE:${POLYHYDRA_EXT}/imgui/backends>"
                           "$<BUILD_INTERFACE:${POLYHYDRA_EXT}/glad/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>")

target_link_libraries(imgui PRIVATE glad glfw)
if (MINGW)
    target_link_libraries(imgui PRIVATE imm32)
endif()

###########################
# stb
###########################

# add stb library
set(stb_DIR ${POLYHYDRA_EXT}/stb)
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
set(tinyfiledialogs_DIR ${POLYHYDRA_EXT}/tinyfiledialogs)
set(tinyfiledialogs_Headers ${tinyfiledialogs_DIR}/tinyfiledialogs.h)
set(tinyfiledialogs_Sources ${tinyfiledialogs_DIR}/tinyfiledialogs.c)

add_library(tinyfiledialogs STATIC ${tinyfiledialogs_Headers} ${tinyfiledialogs_Sources})
target_include_directories(tinyfiledialogs PUBLIC
                           "$<BUILD_INTERFACE:${tinyfiledialogs_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>")

set(polyhydra_SOURCES
        ${POLYHYDRA_SRC}/Window.cpp
        ${POLYHYDRA_SRC}/fs/FileManager.cpp
        ${POLYHYDRA_SRC}/input/Input.cpp
        ${POLYHYDRA_SRC}/mesh/MeshObject.cpp
        ${POLYHYDRA_SRC}/mesh/MeshVertexBuffer.cpp
        ${POLYHYDRA_SRC}/mesh/MeshTextureBuffer.cpp
        ${POLYHYDRA_SRC}/panels/MeshView.cpp
        ${POLYHYDRA_SRC}/panels/LogWindow.cpp
        ${POLYHYDRA_SRC}/panels/ToolBar.cpp
        ${POLYHYDRA_SRC}/panels/NewFileDialog.cpp
        ${POLYHYDRA_SRC}/panels/MeshLayerView.cpp
        ${POLYHYDRA_SRC}/rendering/Renderer.cpp
        ${POLYHYDRA_SRC}/rendering/camera/Camera.cpp
        ${POLYHYDRA_SRC}/rendering/Light.cpp
        ${POLYHYDRA_SRC}/rendering/camera/TrackBall.cpp
        ${POLYHYDRA_SRC}/rendering/gl/Shader.cpp
        ${POLYHYDRA_SRC}/rendering/gl/FrameBufferObject.cpp
        ${POLYHYDRA_SRC}/rendering/gl/PixelBufferObject.cpp
        ${POLYHYDRA_SRC}/rendering/meshes/CylinderMesh.cpp
        ${POLYHYDRA_SRC}/rendering/meshes/ClosedCylinderMesh.cpp
        ${POLYHYDRA_SRC}/rendering/meshes/IcoSphereMesh.cpp
        ${POLYHYDRA_SRC}/rendering/gl/VertexArrayObject.cpp
        ${POLYHYDRA_SRC}/rendering/meshes/CylinderMesh.cpp
        ${POLYHYDRA_SRC}/rendering/passes/MeshPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/BackgroundPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/SelectionPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/SelectionHoverPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/TransparencyPassWB.cpp
        ${POLYHYDRA_SRC}/rendering/passes/VertexOnlyPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/TransparencyPassDP.cpp
        ${POLYHYDRA_SRC}/rendering/passes/ShadowMapPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/PrePass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/SSAOPass.cpp
        ${POLYHYDRA_SRC}/rendering/gl/TextureIcon.cpp
        ${POLYHYDRA_SRC}/settings/AppState.cpp
        ${POLYHYDRA_SRC}/util/UIUtil.cpp
        ${POLYHYDRA_SRC}/util/stb.cpp
        ${POLYHYDRA_SRC}/util/Log.cpp
        ${POLYHYDRA_SRC}/polyhydra.cpp
        ${POLYHYDRA_SRC}/mesh/MeshList.cpp
        ${POLYHYDRA_SRC}/mesh/MeshSerializer.cpp
        ${POLYHYDRA_SRC}/settings/AppState.cpp
        ${POLYHYDRA_SRC}/rendering/passes/GroundPass.cpp
        ${POLYHYDRA_SRC}/rendering/shapes/ShapeRenderer.cpp
        ${POLYHYDRA_SRC}/rendering/shapes/Shapes.cpp
        ${POLYHYDRA_SRC}/rendering/meshes/ArrowMesh.cpp
        ${POLYHYDRA_SRC}/rendering/meshes/ConeMesh.cpp
        ${POLYHYDRA_SRC}/rendering/passes/PostProcessingPass.cpp
        ${POLYHYDRA_SRC}/rendering/passes/OutlinePass.cpp
        ${POLYHYDRA_SRC}/util/RingBuffer.cpp
)

set(polyhydra_HEADERS
        ${POLYHYDRA_INC}/polyhydra/Window.h
        ${POLYHYDRA_INC}/polyhydra/fs/FileManager.h
        ${POLYHYDRA_INC}/polyhydra/input/Input.h
        ${POLYHYDRA_INC}/polyhydra/mesh/MeshObject.h
        ${POLYHYDRA_INC}/polyhydra/mesh/MeshVertexBuffer.h
        ${POLYHYDRA_INC}/polyhydra/mesh/MeshTextureBuffer.h
        ${POLYHYDRA_INC}/polyhydra/panels/MeshView.h
        ${POLYHYDRA_INC}/polyhydra/panels/LogWindow.h
        ${POLYHYDRA_INC}/polyhydra/panels/ToolBar.h
        ${POLYHYDRA_INC}/polyhydra/panels/NewFileDialog.h
        ${POLYHYDRA_INC}/polyhydra/rendering/Light.h
        ${POLYHYDRA_INC}/polyhydra/rendering/Renderer.h
        ${POLYHYDRA_INC}/polyhydra/panels/MeshLayerView.h
        ${POLYHYDRA_INC}/polyhydra/rendering/camera/Camera.h
        ${POLYHYDRA_INC}/polyhydra/rendering/camera/TrackBall.h
        ${POLYHYDRA_INC}/polyhydra/rendering/gl/Shader.h
        ${POLYHYDRA_INC}/polyhydra/rendering/gl/VertexArrayObject.h
        ${POLYHYDRA_INC}/polyhydra/rendering/gl/FrameBufferObject.h
        ${POLYHYDRA_INC}/polyhydra/rendering/gl/PixelBufferObject.h
        ${POLYHYDRA_INC}/polyhydra/rendering/meshes/CylinderMesh.h
        ${POLYHYDRA_INC}/polyhydra/rendering/meshes/ClosedCylinderMesh.h
        ${POLYHYDRA_INC}/polyhydra/rendering/meshes/IcoSphereMesh.h
        ${POLYHYDRA_INC}/polyhydra/rendering/meshes/CylinderMesh.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/RenderPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/MeshPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/BackgroundPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/SelectionPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/VertexOnlyPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/SelectionHoverPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/TransparencyPassWB.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/TransparencyPassDP.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/ShadowMapPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/PrePass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/SSAOPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/gl/TextureIcon.h
        ${POLYHYDRA_INC}/polyhydra/settings/AppState.h
        ${POLYHYDRA_INC}/polyhydra/util/UIUtil.h
        ${POLYHYDRA_INC}/polyhydra/util/Log.h
        ${POLYHYDRA_INC}/polyhydra/polyhydraPCH.h
        ${POLYHYDRA_INC}/polyhydra/polyhydra.h
        ${POLYHYDRA_INC}/polyhydra/mesh/MeshList.h
        ${POLYHYDRA_INC}/polyhydra/util/Types.h
        ${POLYHYDRA_INC}/polyhydra/panels/WindowPanel.h
        ${POLYHYDRA_INC}/polyhydra/mesh/MeshSerializer.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/GroundPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/shapes/ShapeRenderer.h
        ${POLYHYDRA_INC}/polyhydra/rendering/shapes/Shapes.h
        ${POLYHYDRA_INC}/polyhydra/rendering/meshes/ArrowMesh.h
        ${POLYHYDRA_INC}/polyhydra/rendering/meshes/ConeMesh.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/PostProcessingPass.h
        ${POLYHYDRA_INC}/polyhydra/rendering/passes/OutlinePass.h
)

if (MSVC)
        set(POLYHYDRA_RESOURCE_BASE_PATH "..\\\\res\\\\")
else()
        set(POLYHYDRA_RESOURCE_BASE_PATH "./res")
endif()

add_library(polyhydra STATIC ${polyhydra_SOURCES})
add_library(polyhydra::polyhydra ALIAS polyhydra)

### Configure install paths
include(CMakePackageConfigHelpers)
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR "CMake")
else()
  set(DEF_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/MC3D")
endif()
set(polyhydra_INSTALL_CMAKE_DIR "${DEF_INSTALL_CMAKE_DIR}" CACHE PATH "Installation directory for CMake files")
set(polyhydra_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Installation directory for libraries")
set(polyhydra_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Installation directory for header files")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(CONF_LIB_DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
configure_package_config_file(../polyhydraConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/polyhydraConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
# ... for the install tree
set(CONF_INCLUDE_DIRS "${polyhydra_INSTALL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "${polyhydra_INSTALL_LIB_DIR}")
configure_package_config_file(../polyhydraConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/polyhydraConfig.cmake"
  INSTALL_DESTINATION "${polyhydra_INSTALL_CMAKE_DIR}")

target_include_directories(polyhydra
  PUBLIC
  "$<BUILD_INTERFACE:${POLYHYDRA_INC}>"
  "$<INSTALL_INTERFACE:${polyhydra_INSTALL_INCLUDE_DIR}>")

find_package(Filesystem REQUIRED Experimental Final)

target_link_libraries(polyhydra PUBLIC ${CMAKE_DL_LIBS} imgui OpenVolumeMesh::OpenVolumeMesh glm glad glfw nlohmann_json::nlohmann_json stb std::filesystem tinyfiledialogs)

set_target_properties(polyhydra PROPERTIES
                      DEBUG_POSTFIX "d"
                      VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
                      SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON)

target_precompile_headers(polyhydra PUBLIC ${POLYHYDRA_INC}/polyhydra/polyhydraPCH.h)

target_compile_definitions(polyhydra PUBLIC POLYHYDRA_RESOURCE_BASE_PATH="${POLYHYDRA_RESOURCE_BASE_PATH}" FS_HEADER=<${CXX_FILESYSTEM_HEADER}> FS_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE})
# copy our resources to the build dir
set(POLYHYDRA_RES ${POLYHYDRA_ROOT}/res)
file(COPY ${POLYHYDRA_RES} DESTINATION ${CMAKE_BINARY_DIR})

install(DIRECTORY
        "${POLYHYDRA_RES}"
        DESTINATION ${CMAKE_INSTALL_BINDIR})

### Install and export
install(DIRECTORY "${POLYHYDRA_INC}/polyhydra"
        DESTINATION "${C4Hex_INSTALL_INCLUDE_DIR}"
        DESTINATION "${polyhydra_INSTALL_INCLUDE_DIR}")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/polyhydraConfig.cmake"
        DESTINATION "${polyhydra_INSTALL_INCLUDE_DIR}")

install(TARGETS       polyhydra glm glfw glad imgui nlohmann_json stb tinyfiledialogs
        EXPORT        polyhydraTargets
        LIBRARY       DESTINATION "${polyhydra_INSTALL_LIB_DIR}"
        ARCHIVE       DESTINATION "${polyhydra_INSTALL_LIB_DIR}")

install(EXPORT polyhydraTargets
        FILE polyhydraTargets.cmake
        NAMESPACE polyhydra::
        DESTINATION "${polyhydra_INSTALL_CMAKE_DIR}")

export(EXPORT polyhydraTargets
       NAMESPACE polyhydra::)

