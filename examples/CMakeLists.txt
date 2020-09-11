cmake_minimum_required(VERSION 3.10)
project(chip8_lifter_examples)

add_executable(print_ir print_ir.cpp)
target_link_libraries(print_ir chip8::lifter)

add_executable(single_inst single_inst.cpp)
target_link_libraries(single_inst chip8::lifter)

add_executable(to_bitcode to_bitcode.cpp)
target_link_libraries(to_bitcode chip8::lifter)

add_executable(to_object_file to_object_file.cpp)
target_link_libraries(to_object_file chip8::lifter)