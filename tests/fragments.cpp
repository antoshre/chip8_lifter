//
// Created by antoshre on 10/13/20.
//

#include "gtest/gtest.h"

#include "chip8/lifter.h"

#include <cstdint>
#include <vector>
#include <iostream>
#include <span>

void run_fragment(const std::span<std::uint8_t> &program, std::span<std::uint8_t> memory) {
	std::istringstream file(std::string{reinterpret_cast<const char *>(program.data()), program.size()});

	chip8::disasm::Listing l(file);

	auto ctx = std::make_unique<LLVMContext>();
	auto mod = std::make_unique<Module>("module", *ctx);
	chip8::lifter::parse_listing(*mod, l);
	ASSERT_NO_THROW(chip8::lifter::verify_module(*mod));
	chip8::lifter::optimize_module(*mod);
	//chip8::lifter::print_module(*mod);

	//Fragments are mapped at 0x200, so memory must be at least 0x200 + program size large
	ASSERT_GE(memory.size(), 0x200 + program.size());

	auto iter = memory.begin();
	std::advance(iter, 0x200);
	std::copy(program.begin(), program.end(), iter);

	chip8::lifter::run_void_func(std::move(mod), std::move(ctx), "f", memory, false);
}

TEST(Instructions, SETI) {
	std::vector<std::uint8_t> program = {
			0x60, 0xCC, //SETI: set V0 to 0xCC
			0x61, 0xFF, //SETI: set V1 to 0xFF
			//dump registers to memory
			0xA0, 0x00, //ISETI: set I to 0x000
			0xFF, 0x55, //STORE registers 0-F starting at &MEM[I]
	};

	std::array<std::uint8_t, 4096> memory{0};

	run_fragment(program, memory);

	EXPECT_EQ(memory[0], 0xCC);
	EXPECT_EQ(memory[1], 0xFF);
}

TEST(Instructions, ADDI) {
	std::vector<std::uint8_t> program = {
			0x60, 0xCC, //SETI: set V0 to 0xCC
			0x70, 0x33, //ADDI: V0 += 0x33
			//Expect: V0 == 0xFF
			//dump registers to memory
			0xA0, 0x00, //ISETI: set I to 0x000
			0xFF, 0x55, //STORE registers 0-F starting at &MEM[I]
	};

	std::array<std::uint8_t, 4096> memory{0};

	run_fragment(program, memory);

	EXPECT_EQ(memory[0], 0xFF);
}

TEST(Instructions, ADDR) {
	std::vector<std::uint8_t> program = {
			0x60, 0xCC, //SETI: set V0 to 0xCC
			0x61, 0x33, //SETI: set V1 to 0x33
			0x80, 0x14, //ADDR: V0 += V1
			//Expect: V0 = 0xFF, Overflow(Vf) = 0
			//dump registers to memory
			0xA0, 0x00, //ISETI: set I to 0x000
			0xFF, 0x55, //STORE registers 0-F starting at &MEM[I]
	};

	std::array<std::uint8_t, 4096> memory{0};

	run_fragment(program, memory);

	EXPECT_EQ(memory[0], 0xFF);
	EXPECT_EQ(memory[0xF], 0);
}

TEST(Instructions, ADDR_overflow) {
	std::vector<std::uint8_t> program = {
			0x60, 0xCC, //SETI: set V0 to 0xCC
			0x61, 0x34, //SETI: set V1 to 0x34
			0x80, 0x14, //ADDR: V0 += V1
			//Expect: V0 = 0x00, Overflow(Vf) = 1
			//dump registers to memory
			0xA0, 0x00, //ISETI: set I to 0x000
			0xFF, 0x55, //STORE registers 0-F starting at &MEM[I]
	};

	std::array<std::uint8_t, 4096> memory{0};

	run_fragment(program, memory);

	EXPECT_EQ(memory[0], 0x00);
	EXPECT_EQ(memory[0xF], 1);
}