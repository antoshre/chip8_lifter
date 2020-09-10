//
// Created by rob on 9/9/20.
//

#ifndef CHIP8_LIFTER_BLOCKCACHE_H
#define CHIP8_LIFTER_BLOCKCACHE_H

#include <map>

#include "llvm/IR/BasicBlock.h"

using namespace llvm;

namespace chip8::lifter {
	class BlockCache {
		LLVMContext &ctx;
		Function &func;
		std::map<std::string, BasicBlock *> blocks;
		bool locked{false};

	public:
		BlockCache(LLVMContext &, Function &);

		BasicBlock *operator[](const std::string &sv);

		void lock();
	};
}

#endif //CHIP8_LIFTER_BLOCKCACHE_H
