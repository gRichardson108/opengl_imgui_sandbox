cmake_minimum_required(VERSION 3.23)
project(heatmap)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")

include_directories( ${OPENGL_INCLUDE_DIRS} )

# glfw
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory( glfw )
# glad
add_subdirectory( glad )

# Dear ImGui
set(IMGUI_DIR imgui/)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

add_executable(heatmap main.cpp 
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        Shader.cpp Shader.h)

add_dependencies(heatmap copy_resources)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw glad)