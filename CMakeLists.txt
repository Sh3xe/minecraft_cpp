cmake_minimum_required(VERSION 3.16.3)

project(minecraft_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

file( GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/*.cpp )
file( GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/source/*.hpp )

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake_defines.hpp.in
    ${CMAKE_SOURCE_DIR}/source/cmake_defines.hpp )

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} )

add_subdirectory( ${CMAKE_SOURCE_DIR}/dependencies/GLAD )
add_subdirectory( ${CMAKE_SOURCE_DIR}/dependencies/SDL2 )
add_subdirectory( ${CMAKE_SOURCE_DIR}/dependencies/GLM )
add_subdirectory( ${CMAKE_SOURCE_DIR}/dependencies/STB_IMAGE )
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories( minecraft_cpp
	PUBLIC ${CMAKE_SOURCE_DIR}/source)

target_link_libraries( minecraft_cpp
	PUBLIC stb_image
	PUBLIC glad
	PUBLIC glm
	PUBLIC SDL2 
	PUBLIC Threads::Threads
	PUBLIC ${OPENGL_LIBRARIES})