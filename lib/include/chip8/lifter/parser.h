//
// Created by rob on 9/9/20.
//

#ifndef CHIP8_LIFTER_PARSER_H
#define CHIP8_LIFTER_PARSER_H

#include <memory>
#include <llvm/IR/IRBuilder.h>

#include "llvm/IR/Module.h"

#include "chip8/disasm/Listing.h"
#include "chip8/lifter/parser.h"
#include "BuilderHelper.h"

namespace chip8::lifter {

	namespace detail {
		struct Chip8Machine {
			Chip8Machine(llvm::IRBuilder<> &b, chip8::lifter::BuilderHelper &bh);

			llvm::Value *V; //16 8 bit GP regs
			llvm::Value *I; //16 bit
			llvm::Value *PC;    //16 bit

			llvm::Value *SP;    //8 bit
			llvm::Value *stack; //16 16 bit

			llvm::Value *MEM;   //byte-addressed 4k

			llvm::Value *delay; //8 bits
			llvm::Value *sound; //8 bits

			llvm::Function *rand;
		};
	}

	void parse_listing(llvm::Module &, const chip8::disasm::Listing &);
}

#endif //CHIP8_LIFTER_PARSER_H
