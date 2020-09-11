//
// Created by rob on 9/10/20.
//

#include <iostream>
#include <fstream>
#include <memory>

#include "chip8/disasm.h"
#include "chip8/lifter.h"

using namespace chip8::disasm;

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetParser.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/LegacyPassManager.h"

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

	auto has_indirect_jump = chip8::lifter::passes::check_for_indirect_jumps(l);
	if (has_indirect_jump) {
		std::cout << "Program contains indirect jump, cannot statically lift.  Sorry.\n";
		return 3;
	} else {
		std::cout << "No indirect jumps found, continuing\n";
	}

	auto ctx = std::make_unique<LLVMContext>();
	auto mod = std::make_unique<Module>("module", *ctx);

	chip8::lifter::parse_listing(*mod, l);
	chip8::lifter::verify_module(*mod);
	chip8::lifter::optimize_module(*mod);

	auto target_triple = llvm::sys::getDefaultTargetTriple();
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();
	std::string Error;
	auto Target = TargetRegistry::lookupTarget(target_triple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
		errs() << Error;
		return 1;
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = Optional<Reloc::Model>();
	auto TargetMachine = Target->createTargetMachine(target_triple, CPU, Features, opt, RM);


	mod->setDataLayout(TargetMachine->createDataLayout());
	mod->setTargetTriple(target_triple);

	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OpenFlags::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		return 1;
	}

	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TargetMachine can't emit a file of this type";
		return 1;
	}

	pass.run(*mod);
	dest.flush();

	return 0;
}