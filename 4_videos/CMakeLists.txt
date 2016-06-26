cmake_minimum_required(VERSION 2.6)
project(glmixer)

set (glmixer_VERSION_MAJOR 1)
set (glmixer_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/glmixer_config.h.in"
  "${PROJECT_BINARY_DIR}/glmixer_config.h"
)

configure_file (
  "${PROJECT_SOURCE_DIR}/vertex_shader.vert"
  "${PROJECT_BINARY_DIR}/vertex_shader.vert"
)

configure_file (
  "${PROJECT_SOURCE_DIR}/fragment_shader.frag"
  "${PROJECT_BINARY_DIR}/fragment_shader.frag"
)


find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
  "${PROJECT_BINARY_DIR}"
  "${OpenCV_INCLUDE_DIRS}"
  "${GLEW_INCLUDE_DIRS}"
)

add_compile_options("-std=c++11" "-Wall" "-Wextra")
add_executable(glmixer main.cpp)
target_link_libraries (glmixer 
  "${OpenCV_LIBRARIES}"
  "${GLEW_LIBRARIES}"
   glfw
  "${OPENGL_LIBRARIES}"
)

# install(TARGETS glmixer 
#   RUNTIME DESTINATION bin
# )
