cmake_minimum_required(VERSION 3.5)

project(Game)

add_executable(Game "main.cpp")

target_link_libraries(Game PUBLIC project_options project_warnings Engine)

#target_link_libraries(Example PUBLIC CONAN_PKG::glew CONAN_PKG::glfw CONAN_PKG::imgui imgui_bindings)
