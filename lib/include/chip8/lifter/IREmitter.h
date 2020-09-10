//
// Created by rob on 9/9/20.
//

#ifndef CHIP8_LIFTER_IREMITTER_H
#define CHIP8_LIFTER_IREMITTER_H

#include "BuilderHelper.h"
#include "parser.h"
#include "chip8/disasm/Instructions.h"

namespace chip8::lifter::emitter {
	using namespace chip8::disasm::instruction;

	struct IREmitter {
		detail::Chip8Machine &m;
		BuilderHelper &b;

		explicit IREmitter(detail::Chip8Machine &, BuilderHelper &);

		void operator()(const SYS &);

		void operator()(const CLS &);

		void operator()(const RET &);

		void operator()(const JUMPI &);

		void operator()(const CALLI &);

		void operator()(const SKEI &);

		void operator()(const SKNEI &);

		void operator()(const SKER &);

		void operator()(const ADDI &);

		void operator()(const SETI &);

		void operator()(const SETR &);

		void operator()(const ORR &);

		void operator()(const ANDR &);

		void operator()(const XORR &);

		void operator()(const ADDR &);

		void operator()(const SUBR &);

		void operator()(const SUBR_R &);

		void operator()(const SHR &);

		void operator()(const SHL &);

		void operator()(const SKNER &);

		void operator()(const ISETI &);

		void operator()(const JUMPIND &);

		void operator()(const RAND &);

		void operator()(const DRAW &);

		void operator()(const SKEK &);

		void operator()(const SKNEK &);

		void operator()(const GETD &);

		void operator()(const KEY &);

		void operator()(const SETD &);

		void operator()(const SETS &);

		void operator()(const IADDR &);

		void operator()(const SPRITE &);

		void operator()(const BCD &);

		void operator()(const STORE &);

		void operator()(const LOAD &);

	};
}

#endif //CHIP8_LIFTER_IREMITTER_H
