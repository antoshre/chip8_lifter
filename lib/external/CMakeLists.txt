cmake_minimum_required(VERSION 3.10)
project(external_deps)

include(FetchContent)

if (NOT TARGET chip8_disassembler_lib)
	FetchContent_Declare(
			chip8_disassembler_lib
			GIT_REPOSITORY https://github.com/antoshre/chip8_disassembler.git
	)
	FetchContent_MakeAvailable(chip8_disassembler_lib) #chip8::disasm alias
endif ()
#if (NOT TARGET frozen)
#    FetchContent_Declare(
#            frozen
#            GIT_REPOSITORY https://github.com/serge-sans-paille/frozen.git
#    )
#    FetchContent_MakeAvailable(frozen)
#endif()
#
#if (NOT TARGET ctre)
#    FetchContent_Declare(
#            ctre
#            GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
#    )
#    FetchContent_MakeAvailable(ctre)
#    #CTRE requires C++20
#    target_compile_features(ctre INTERFACE cxx_std_20)
#endif()