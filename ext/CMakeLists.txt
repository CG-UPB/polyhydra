###########################
# Git Submodules
###########################

# update all submodules before building
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${POLYHYDRA_ROOT}/.git")
    # Update submodules as needed
    message(STATUS "Submodule init")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -f
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/OpenVolumeMesh/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glm/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/json-develop/CMakeLists.txt"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image_write.h"
        OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tinyfiledialogs/tinyfiledialogs.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
