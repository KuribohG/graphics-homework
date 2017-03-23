cmake_minimum_required(VERSION 3.6)
project(hw2)

include(FindOpenGL)
include(FindGLUT)

set(CMAKE_CXX_STANDARD 11)

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

set(SOURCE_FILES main.cpp)
add_executable(hw2 ${SOURCE_FILES})
target_link_libraries(hw2 ${LIBRARIES})