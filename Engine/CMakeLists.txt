cmake_minimum_required(VERSION 3.5)

project(Engine)

file(GLOB_RECURSE SOURCES . ./*.cpp)

add_library(Engine STATIC ${SOURCES})

target_link_libraries(Engine PUBLIC 
    CONAN_PKG::spdlog 
    CONAN_PKG::glm 
    CONAN_PKG::glew 
    CONAN_PKG::glfw 
    CONAN_PKG::imgui
    CONAN_PKG::jsoncpp)
    
target_link_libraries(Engine PUBLIC soil imgui_bindings)
target_link_libraries(Engine PUBLIC project_options project_warnings)

target_include_directories(Engine PUBLIC ".")
