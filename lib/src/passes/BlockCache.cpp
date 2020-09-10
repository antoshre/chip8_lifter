//
// Created by rob on 9/9/20.
//

#include "chip8/lifter/BlockCache.h"

namespace chip8::lifter {
	BlockCache::BlockCache(LLVMContext &c, Function &f) : ctx(c), func(f) {}

	BasicBlock *BlockCache::operator[](const std::string &sv) {
		const std::string temp(sv);
		if (blocks.find(temp) != blocks.end()) {
			return blocks[temp];
		} else {
			if (!locked) {
				auto it = blocks.emplace(temp, BasicBlock::Create(ctx, temp, &func));
				return it.first->second;
			} else {
				throw std::runtime_error("Block cache is locked, cannot create new block named: " + temp);
			}
		}
	}

	void BlockCache::lock() {
		locked = true;
	}
}