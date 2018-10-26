cmake_minimum_required(VERSION 2.8)

file(
		GLOB
		source_files
		./*.cpp
)

INCLUDE_DIRECTORIES(.)

add_executable (template_snake main.cpp)
