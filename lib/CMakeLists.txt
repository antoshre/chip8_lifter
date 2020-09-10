cmake_minimum_required(VERSION 3.10)
project(chip8_lifter_lib)

add_subdirectory(external)

find_package(LLVM "10.0.0" REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_library(${PROJECT_NAME} src/source.cpp
		src/passes/check_for_indirect.cpp include/chip8/lifter/passes/check_for_indirect.h src/parser.cpp src/passes/BlockCache.cpp src/IREmitter.cpp src/BuilderHelper.cpp
		src/ModuleTools.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src PUBLIC include)

target_link_libraries(${PROJECT_NAME} chip8::disasm LLVM)
target_include_directories(${PROJECT_NAME} PUBLIC include ${LLVM_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${LLVM_DEFINITIONS})

add_library(chip8::lifter ALIAS ${PROJECT_NAME})