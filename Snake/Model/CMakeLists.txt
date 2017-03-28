cmake_minimum_required(VERSION 2.8)

file(
		GLOB
		source_files
		./*.cpp
)

ADD_LIBRARY (model ${source_files} ../../src/AI.cpp)
