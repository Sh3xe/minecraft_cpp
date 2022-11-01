cmake_minimum_required(VERSION 3.16)

project(stb_image LANGUAGES C)

add_library(stb_image SHARED src/stb_image.c)
target_include_directories(stb_image PUBLIC include)