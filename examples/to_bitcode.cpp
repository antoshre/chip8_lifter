//
// Created by antoshre on 9/10/20.
//

#include "chip8/lifter.h"

#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/raw_os_ostream.h"

#include <cstdint>
#include <iostream>
#include <fstream>

using namespace llvm;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage: [filename.ch8]" << std::endl;
		return 1;
	}
	std::ifstream file(argv[1], std::ios::binary | std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Cannot open file: " << argv[1] << std::endl;
		return 2;
	}

	chip8::disasm::Listing l(file);

	std::cout << "Listing:\n" << l << '\n';

	auto ctx = std::make_unique<LLVMContext>();
	auto mod = std::make_unique<Module>("module", *ctx);

	chip8::lifter::parse_listing(*mod, l);
	chip8::lifter::verify_module(*mod);
	chip8::lifter::optimize_module(*mod);

	std::error_code ec;
	std::string output_name = argv[1];
	output_name += ".bc";

	std::ofstream ofile(output_name, std::ios::binary | std::ios::out);
	if (!ofile.is_open()) {
		std::cerr << "Cannot open output file: " << output_name << std::endl;
		return 3;
	}
	llvm::raw_os_ostream ostream(ofile);
	WriteBitcodeToFile(*mod, ostream);

	std::cout << "Wrote bitcode to " << output_name << std::endl;

	return 0;
}