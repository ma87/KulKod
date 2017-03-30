cmake_minimum_required(VERSION 2.8)

file(
		GLOB
		source_files
		./*.cpp
)

INCLUDE_DIRECTORIES(.)

ADD_LIBRARY (template_snake SHARED ${source_files})
