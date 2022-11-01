cmake_minimum_required(VERSION 3.16)

project(glad LANGUAGES C)

add_library(glad SHARED src/glad.c)
target_include_directories(glad PUBLIC include)