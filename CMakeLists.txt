cmake_minimum_required(VERSION 3.28)

project(OpenGLTutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main ./src/main.cpp ./src/glad.c ./src/shader.cpp)
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(main PUBLIC glfw GL X11 pthread Xrandr) 