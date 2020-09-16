//
// Created by antoshre on 9/9/20.
//

#include "chip8/lifter.h"

#include <cstdint>
#include <sstream>
#include <iostream>

int main() {

	//7xkk - ADD Vx, byte
	//Set Vx = Vx + kk.
	//
	//Adds the value kk to the value of register Vx, then stores the result in Vx.

	//Annn : I = nnn
	//6xkk : V[x] = kk
	//7xkk : V[x] += kk
	//8xy4 : V[x] += V[y]
	//Dxyn - DRW Vx, Vy, nibble
	//Fx55 : store V0-Vx into [I]
	//Fx0A - LD Vx, Keypad
	std::vector<std::uint8_t> program = {
			0x62, 0x00,
			0x63, 0x00,
			0xA2, 0x0A,
			0xD2, 0x36,
			//0xFF, 0x55, //STORE(V0-V3)
	};

	std::istringstream file(std::string{reinterpret_cast<const char *>(program.data()), program.size()});

	chip8::disasm::Listing l(file, 0x200);

	std::cout << "Listing:\n" << l << '\n';

	auto ctx = std::make_unique<LLVMContext>();
	auto mod = std::make_unique<Module>("module", *ctx);

	chip8::lifter::parse_listing(*mod, l);

	chip8::lifter::verify_module(*mod);

	//chip8::lifter::print_module(*mod);

	//std::cout << "Optimized:" << std::endl;
	chip8::lifter::optimize_module(*mod);
	chip8::lifter::print_module(*mod);

	std::array<std::uint8_t, 4096> memory{0};

	//write in character spriteset
	//0-F in ascending order
	//'0' : [0,4]
	//'1' : [5,9]
	std::array<std::uint8_t, 80> charsprites = {
			0xF0, 0x90, 0x90, 0x90, 0xF0,
			0x20, 0x60, 0x20, 0x20, 0x70,
			0xF0, 0x10, 0xF0, 0x80, 0xF0,
			0xF0, 0x10, 0xF0, 0x10, 0xF0,
			0x90, 0x90, 0xF0, 0x10, 0x10,
			0xF0, 0x80, 0xF0, 0x10, 0xF0,
			0xF0, 0x80, 0xF0, 0x90, 0xF0,
			0xF0, 0x10, 0x20, 0x40, 0x40,
			0xF0, 0x90, 0xF0, 0x90, 0xF0,
			0xF0, 0x90, 0xF0, 0x10, 0xF0,
			0xF0, 0x90, 0xF0, 0x90, 0x90,
			0xE0, 0x90, 0xE0, 0x90, 0xE0,
			0xF0, 0x80, 0x80, 0x80, 0xF0,
			0xE0, 0x90, 0x90, 0x90, 0xE0,
			0xF0, 0x80, 0xF0, 0x80, 0xF0,
			0xF0, 0x80, 0xF0, 0x80, 0x80
	};
	std::copy(charsprites.begin(), charsprites.end(), memory.begin());


	//copy program data into memory
	//Program opcodes aren't used, but the sprite data in the rom will
	//ROMs loaded starting at 0x200 by convention
	auto iter = memory.begin();
	std::advance(iter, 0x200);
	std::copy(program.begin(), program.end(), iter);

	chip8::lifter::run_void_func(std::move(mod), std::move(ctx), "f", memory);
	return 0;
}