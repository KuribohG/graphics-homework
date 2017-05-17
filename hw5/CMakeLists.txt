cmake_minimum_required(VERSION 3.6)
project(hw5)

include(FindOpenGL)
include(FindGLUT)
include(FindGLEW)

set(CMAKE_CXX_STANDARD 11)

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})

set(SOURCE_FILES main.cpp)
add_executable(hw5 ${SOURCE_FILES})
add_custom_command(
        TARGET hw5 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/bump_mapping.vert
        ${CMAKE_CURRENT_BINARY_DIR}/bump_mapping.vert)
add_custom_command(
        TARGET hw5 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/bump_mapping.frag
        ${CMAKE_CURRENT_BINARY_DIR}/bump_mapping.frag)
add_custom_command(
        TARGET hw5 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/brick_texture.txt
        ${CMAKE_CURRENT_BINARY_DIR}/brick_texture.txt)
add_custom_command(
        TARGET hw5 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/brick.txt
        ${CMAKE_CURRENT_BINARY_DIR}/brick.txt)
target_link_libraries(hw5 ${LIBRARIES})
