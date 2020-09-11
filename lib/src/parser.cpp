//
// Created by antoshre on 9/9/20.
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

		auto rand_func = module.getOrInsertFunction("random_byte", IntegerType::getInt8Ty(ctx));
		auto rand = cast<Function>(rand_func.getCallee());

		auto draw_func = module.getOrInsertFunction("draw", IntegerType::getInt1Ty(ctx), IntegerType::getInt8Ty(ctx),
		                                            IntegerType::getInt8Ty(ctx), IntegerType::getInt8PtrTy(ctx),
		                                            IntegerType::getInt8Ty(ctx));
		auto draw = cast<Function>(draw_func.getCallee());

		auto clear_screen_func = module.getOrInsertFunction("clear_screen", IntegerType::getVoidTy(ctx));
		auto clear_screen = cast<Function>(clear_screen_func.getCallee());

		auto poll_keypad_func = module.getOrInsertFunction("poll_keypad", IntegerType::getInt16Ty(ctx));
		auto poll_keypad = cast<Function>(poll_keypad_func.getCallee());

		auto block_keypad_func = module.getOrInsertFunction("block_keypad", IntegerType::getInt16Ty(ctx));
		auto block_keypad = cast<Function>(block_keypad_func.getCallee());

		BlockCache bcache(ctx, *foo);
		IRBuilder<> b(ctx);
		b.SetInsertPoint(bcache["entry"]);
		BuilderHelper bh(b, ctx, bcache);

		detail::Chip8Machine machine{b, bh};
		machine.rand = rand;
		machine.draw = draw;
		machine.clear_screen = clear_screen;
		machine.poll_keypad = poll_keypad;
		machine.block_keypad = block_keypad;

		auto foo_args = foo->arg_begin();
		machine.MEM = foo_args++;
		machine.MEM->setName("MEM");


		for (auto &&inst : l.instructions) {
			std::visit(emitter::IREmitter{machine, bh}, inst);
		}

		b.CreateRetVoid();
	}
}