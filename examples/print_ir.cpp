//
// Created by antoshre on 9/3/20.
//
#include <iostream>
#include <fstream>
#include <memory>

#include "chip8/disasm.h"
#include "chip8/lifter.h"

using namespace chip8::disasm;

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;


int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage: [file.ch8]" << std::endl;
		return 1;
	}
	std::ifstream file(argv[1], std::ios::binary | std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Cannot open file: " << argv[1] << std::endl;
		return 2;
	}

	Listing l(file);

	std::cout << l << std::endl;

	auto has_indirect_jump = chip8::lifter::passes::check_for_indirect_jumps(l);
	if (has_indirect_jump) {
		std::cout << "Program contains indirect jump, cannot statically lift.  Sorry.\n";
		return 3;
	}

	auto ctx = std::make_unique<LLVMContext>();
	auto mod = std::make_unique<Module>("module", *ctx);

	chip8::lifter::parse_listing(*mod, l);
	chip8::lifter::verify_module(*mod);
	chip8::lifter::optimize_module(*mod);
	chip8::lifter::print_module(*mod);
	return 0;
}