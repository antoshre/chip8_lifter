cmake_minimum_required(VERSION 3.10)
project(chip8_lifter)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

option(BUILD_EXAMPLES "Build examples" ON)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_EXAMPLES)
	add_subdirectory(examples)
endif ()

option(BUILD_TESTING "Build tests" ON)
option(ENABLE_COVERAGE "Generate coverage statistics" OFF)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif ()