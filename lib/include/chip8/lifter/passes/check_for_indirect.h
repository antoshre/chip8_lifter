//
// Created by rob on 9/9/20.
//

#ifndef CPP_TEMPLATE_CHECK_FOR_INDIRECT_H
#define CPP_TEMPLATE_CHECK_FOR_INDIRECT_H

#include "chip8/disasm.h"

namespace chip8::lifter::passes {
	bool check_for_indirect_jumps(const chip8::disasm::Listing &);
}

#endif //CPP_TEMPLATE_CHECK_FOR_INDIRECT_H
