//
// Created by rob on 9/9/20.
//

#include "chip8/lifter/parser.h"
#include "chip8/lifter/BlockCache.h"
#include "chip8/lifter/BuilderHelper.h"
#include "chip8/lifter/IREmitter.h"

#include <array>
#include <cstdint>

#include "chip8/embedded/runtime.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"

using namespace llvm;

namespace chip8::lifter {

	detail::Chip8Machine::Chip8Machine(IRBuilder<> &b, BuilderHelper &bh) {
		auto &ctx = b.getContext();
		Type *int8 = IntegerType::getInt8Ty(ctx);
		Type *int16 = IntegerType::getInt16Ty(ctx);
		V = b.CreateAlloca(int8, bh.i8(16), "V registers");
		I = b.CreateAlloca(int16, nullptr, "I register");
		PC = b.CreateAlloca(int16, nullptr, "program counter");
		SP = b.CreateAlloca(int8, nullptr, "stack pointer");
		stack = b.CreateAlloca(int16, bh.i16(16), "stack");

		delay = b.CreateAlloca(int8, nullptr, "delay register");
		sound = b.CreateAlloca(int8, nullptr, "sound register");
	}

	void parse_listing(llvm::Module &module, const chip8::disasm::Listing &l) {
		auto &ctx = module.getContext();
		auto func = module.getOrInsertFunction("f", IntegerType::getVoidTy(ctx), IntegerType::getInt8PtrTy(ctx));
		auto foo = cast<Function>(func.getCallee());

		////Initial support for keyboard and screen instrumentation
		//auto keyboard_func = module.getOrInsertFunction("handle_keyboard", IntegerType::getVoidTy(ctx),
		//                                                        IntegerType::getInt16PtrTy(ctx));
		//auto keyboard = cast<Function>(keyboard_func.getCallee());

		auto rand_func = module.getOrInsertFunction("chip8::runtime::rand", IntegerType::getInt8Ty(ctx));
		auto rand = cast<Function>(rand_func.getCallee());

		BlockCache bcache(ctx, *foo);
		IRBuilder<> b(ctx);
		b.SetInsertPoint(bcache["entry"]);
		BuilderHelper bh(b, ctx, bcache);

		detail::Chip8Machine machine{b, bh};
		machine.rand = rand;

		auto foo_args = foo->arg_begin();
		machine.MEM = foo_args++;
		machine.MEM->setName("MEM");


		for (auto &&inst : l.instructions) {
			std::visit(emitter::IREmitter{machine, bh}, inst);
		}

		b.CreateRetVoid();


	}
}