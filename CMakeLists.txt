cmake_minimum_required(VERSION 3.19.2)
project(minecraft_cpp)

link_directories(${CMAKE_SOURCE_DIR}/dependencies/lib)

file( GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/source/*.cpp
    ${CMAKE_SOURCE_DIR}/source/*.c )

file( GLOB_RECURSE HEADER_FILES
    ${CMAKE_SOURCE_DIR}/source/*.hpp
    ${CMAKE_SOURCE_DIR}/source/*.h )


configure_file(
    ${CMAKE_SOURCE_DIR}/source/_configuration/cmake_defines.hpp.in
    ${CMAKE_SOURCE_DIR}/source/cmake_defines.hpp )


add_executable( ${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} )

include_directories(
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/dependencies/src
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/cmake/")

find_package(OpenGL REQUIRED)
message("Opengl found")

find_package(GLM REQUIRED)
message("Glm found")

find_package(GLFW3 REQUIRED)
message("Glfw3 found")

add_library(STB_IMAGE "dependencies/src/stb_image.cpp")
message("Stb_image found")

add_library(GLAD "dependencies/src/glad.c")
message("Glad found")

set(LIBS glfw3 opengl32 STB_IMAGE GLAD)

target_link_libraries(${PROJECT_NAME} ${LIBS})