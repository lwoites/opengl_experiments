cmake_minimum_required(VERSION 2.6)
project(add_rm_video)

set (add_rm_video_VERSION_MAJOR 1)
set (add_rm_video_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/add_rm_video_config.h.in"
  "${PROJECT_BINARY_DIR}/add_rm_video_config.h"
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


include_directories(
  "${PROJECT_BINARY_DIR}"
  "${OpenCV_INCLUDE_DIRS}"
  "${GLEW_INCLUDE_DIRS}"
)

add_compile_options("-std=c++11" "-Wall" "-Wextra")
add_executable(add_rm_video 
    main.cpp
    framesource.cpp
    gl_utils.cpp 
)

target_link_libraries (add_rm_video 
  "${OpenCV_LIBRARIES}"
  "${GLEW_LIBRARIES}"
   glfw
)

# install(TARGETS add_rm_video 
#   RUNTIME DESTINATION bin
# )
