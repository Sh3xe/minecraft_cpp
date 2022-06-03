cmake_minimum_required(VERSION 3.16.3)

project(minecraft_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if( WIN32 )
	add_compile_definitions( MC_WINDOWS )
else()
	add_compile_definitions( MC_LINUX )
endif()

link_directories(${CMAKE_SOURCE_DIR}/dependencies/lib)

file( GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/*.cpp )
file( GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/source/*.hpp )

configure_file(
    ${CMAKE_SOURCE_DIR}/source/_configuration/cmake_defines.hpp.in
    ${CMAKE_SOURCE_DIR}/source/cmake_defines.hpp )

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} )

include_directories(
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/dependencies/src )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/cmake/")
set(SDL2_PATH  "${CMAKE_SOURCE_DIR}/dependencies")

find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2 REQUIRED)

add_library(STB_IMAGE "dependencies/src/stb_image.cpp")

if( WIN32 )
	set( GLEW_LIBS -l${CMAKE_SOURCE_DIR}/dependencies/lib/x64/glew32.lib -l${CMAKE_SOURCE_DIR}/dependencies/lib/x64/glew32s.lib )
	set( GLEW_INCLUDE ${CMAKE_SOURCE_DIR}/dependencies/include )
	include_directories( ${GLEW_INCLUDE} )
	
	set( LIBS ${SDL2_LIBRARY} STB_IMAGE ${GLEW_LIBS} )
else()
	find_package(GLEW REQUIRED)
	set(LIBS ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} STB_IMAGE ${GLEW_LIBRARIES} pthread )
endif()

target_link_libraries(${PROJECT_NAME} ${LIBS})