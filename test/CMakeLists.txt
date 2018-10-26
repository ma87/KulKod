cmake_minimum_required (VERSION 2.8)

add_executable(test_launcher main.c)
add_executable(simple_test   simple_test.c)

target_link_libraries(test_launcher KulKod)
