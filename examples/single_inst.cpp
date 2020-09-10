//
// Created by rob on 9/9/20.
//

#include "chip8/lifter.h"

#include <cstdint>
#include <strstream>
#include <iostream>

int main() {

	//7xkk - ADD Vx, byte
	//Set Vx = Vx + kk.
	//
	//Adds the value kk to the value of register Vx, then stores the result in Vx.

	//Annn : I = nnn
	//6xkk : V[x] = kk
	//7xkk : V[x] += kk
	//Fx55 : store V0-Vx into [I]
	std::vector<std::uint8_t> program = {
			0xA0, 0x00, //I = 0
			0x60, 0xCC, //V0 = 0xCC
			0x61, 0xBB,
			0x62, 0xA1,
			//0xC1, 0x44, //V1 = rand() & 0x44
			0xF2, 0x55, //STORE(V0-V3)
	};
	std::istrstream file = {reinterpret_cast<const char *>(program.data()),
	                        static_cast<std::streamsize>(program.size())};

	chip8::disasm::Listing l(file);

	std::cout << "Listing:\n" << l << '\n';

	auto ctx = std::make_unique<LLVMContext>();
	auto mod = std::make_unique<Module>("module", *ctx);

	chip8::lifter::parse_listing(*mod, l);

	chip8::lifter::verify_module(*mod);

	//chip8::lifter::print_module(*mod);

	std::cout << "Optimized:" << std::endl;
	chip8::lifter::optimize_module(*mod);
	chip8::lifter::print_module(*mod);

	std::array<std::uint8_t, 4096> memory{};

	//copy program data into memory
	//Program opcodes aren't used, but the sprite data in the rom will
	auto iter = memory.begin();
	std::advance(iter, 0x200);
	std::copy(program.begin(), program.end(), iter);

	chip8::lifter::run_void_func(std::move(mod), std::move(ctx), "f", memory);
	return 0;
}