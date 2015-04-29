cmake_minimum_required(VERSION 2.6)
project(change_properties)

configure_file (
  "${PROJECT_SOURCE_DIR}/vertex_shader.vert"
  "${PROJECT_BINARY_DIR}/vertex_shader.vert"
)

configure_file (
  "${PROJECT_SOURCE_DIR}/fragment_shader.frag"
  "${PROJECT_BINARY_DIR}/fragment_shader.frag"
)


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


include_directories(
  "${PROJECT_BINARY_DIR}"
  "${OPENGL_INCLUDE_DIR}"
  "${GLEW_INCLUDE_DIRS}"
)

add_compile_options("-std=c++11" "-Wall" "-Wextra")
add_executable(change_properties main.cpp)
target_link_libraries (change_properties 
  "${OPENGL_LIBRARIES}"  
  "${GLEW_LIBRARIES}"
   glfw
)

# install(TARGETS change_properties 
#   RUNTIME DESTINATION bin
# )
