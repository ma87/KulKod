cmake_minimum_required (VERSION 2.8)

project(SNAKE)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")


INCLUDE_DIRECTORIES(./Model)


add_subdirectory(Model)
add_subdirectory(TemplateSnake)
add_subdirectory(MCSnake)

add_executable (snake main.cpp)
target_link_libraries (snake model KulKod::Launcher KulKod::EnergyMeasurement)
