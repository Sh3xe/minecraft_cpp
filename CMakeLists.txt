cmake_minimum_required(VERSION 3.16.3)
  
project(minecraft_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

link_directories(${CMAKE_SOURCE_DIR}/dependencies/lib)

file( GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/*.cpp )

configure_file(
    ${CMAKE_SOURCE_DIR}/source/_configuration/cmake_defines.hpp.in
    ${CMAKE_SOURCE_DIR}/source/cmake_defines.hpp )


add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

include_directories(
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/dependencies/src
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/cmake/")
set(SDL2_DIR  "${CMAKE_SOURCE_DIR}/dependencies")

find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2 REQUIRED)

add_library(STB_IMAGE "dependencies/src/stb_image.cpp")
message("Stb_image found")

add_library(GLAD "dependencies/src/glad.c")
set(LIBS ${SDL2_LIBRARY} STB_IMAGE  GLAD )

target_link_libraries(${PROJECT_NAME} ${LIBS})