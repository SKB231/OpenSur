cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)

project(OpenGLTutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB IMGUI_SOURCES "${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp" "${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3*" "${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw*")

add_executable(main ./main.cpp ./src/glad.c ./src/shader.cpp ./src/camera.cpp ${IMGUI_SOURCES} ./src/model.cpp ./src/mesh.cpp)

target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/vendor)
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/vendor/imgui)
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/vendor/imgui/backends)
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/include)
if(APPLE) 
    target_include_directories(main PUBLIC /opt/homebrew/opt/glfw/include)
    target_include_directories(main PUBLIC /opt/homebrew/opt/glm/include)
endif()

target_link_libraries(main PUBLIC glfw ${CMAKE_SOURCE_DIR}/vendor/assimp/libassimp.5.4.3.dylib) 
if(NOT APPLE)
    target_link_libraries(main PUBLIC GL X11 pthread Xrandr)
endif()

