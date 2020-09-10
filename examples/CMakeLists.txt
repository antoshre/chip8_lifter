cmake_minimum_required(VERSION 3.10)
project(cpp_template_examples)

add_executable(example example.cpp)
target_link_libraries(example chip8::lifter)

add_executable(single_inst single_inst.cpp)
target_link_libraries(single_inst chip8::lifter)