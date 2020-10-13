cmake_minimum_required(VERSION 3.10)
project(tests)

add_executable(all_tests source.cpp fragments.cpp)
target_link_libraries(all_tests chip8::lifter)

find_package(GTest)
if (NOT GTEST_FOUND)
	message(STATUS "GTest package not found, pulling from remote")
	include(FetchContent)
	FetchContent_Declare(
			googletest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG release-1.10.0
	)
	#Populate gtest gtest_main targets of googletest project
	FetchContent_MakeAvailable(googletest)

endif ()

target_link_libraries(all_tests gtest gtest_main)

if (ENABLE_COVERAGE)
	target_compile_options(all_tests PUBLIC --coverage -fprofile-arcs -ftest-coverage)
	target_link_options(all_tests PUBLIC --coverage -fprofile-arcs -ftest-coverage)
endif ()



