//
// Created by rob on 9/9/20.
//

#include "chip8/lifter/passes/check_for_indirect.h"

#include <algorithm>
#include <variant>

namespace chip8::lifter::passes {
	using chip8::disasm::instruction::JUMPIND;

	bool check_for_indirect_jumps(const chip8::disasm::Listing &l) {
		return std::any_of(l.instructions.begin(), l.instructions.end(),
		                   [](auto &&i) { return std::holds_alternative<JUMPIND>(i); });
	}
}