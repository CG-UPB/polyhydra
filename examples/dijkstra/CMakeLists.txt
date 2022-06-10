cmake_minimum_required(VERSION 3.12)

get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME "volumeshOS_${MODULE_NAME}")
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} Dijkstra.cpp main.cpp)

target_link_libraries(${PROJECT_NAME} volumeshOS_core)

# this is only needed for examples
get_target_property(${PROJECT_NAME}_BINARY_DIR ${PROJECT_NAME} BINARY_DIR)
file(COPY ${VOLUMESHOS_RES} DESTINATION ${${PROJECT_NAME}_BINARY_DIR})