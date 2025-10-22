cmake_minimum_required(VERSION 4.0)
project(projeto-1 C)

set(CMAKE_C_STANDARD 11)

add_library(meditor STATIC meditor.c)

add_executable(projeto-main main.c
        command-line-meditor.c)

target_link_libraries(projeto-main PRIVATE meditor)

target_include_directories(projeto-main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
