//
// Created by rob on 9/9/20.
//

#include "chip8/lifter/IREmitter.h"

#include <iostream>

namespace chip8::lifter::emitter {
	IREmitter::IREmitter(detail::Chip8Machine &_m, BuilderHelper &_bh) : m(_m), b(_bh) {}

	void IREmitter::operator()(const SYS &i) {
		//Jump to a machine code routine at nnn.
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const CLS &i) {
		std::clog << "CLS not implemented, skipping...\n";
		//throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const RET &i) {
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const JUMPI &i) {
		//Return from a subroutine.
		//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
		//throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
		//auto sp = b.bldr.CreateLoad(m.SP);  //sp = m.SP
		//auto val = b.read_array(m.stack, sp); //val = m.stack[m.SP]
		//b.bldr.CreateStore(val, m.PC);    //PC = val
		//auto res = b.op_sub(sp, 1);
		//b.bldr.CreateStore(res, m.SP);
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const CALLI &i) {
		//2nnn - CALL addr
		//Call subroutine at nnn.
		//The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SKEI &i) {
		//3xkk - SE Vx, byte
		//Skip next instruction if Vx = kk.
		//The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SKNEI &i) {
		//4xkk - SNE Vx, byte
		//Skip next instruction if Vx != kk.
		//The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SKER &i) {
		//5xy0 - SE Vx, Vy
		//Skip next instruction if Vx = Vy.
		//The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SETI &i) {
		//6xkk - LD Vx, byte
		//Set Vx = kk.
		//The interpreter puts the value kk into register Vx.
		b.write_array(m.V, i.x(), i.kk());
	}

	void IREmitter::operator()(const ADDI &i) {
		//7xkk - ADD Vx, byte
		//Set Vx = Vx + kk.
		//Adds the value kk to the value of register Vx, then stores the result in Vx.
		auto vx = b.read_array(m.V, i.x());
		auto res = b.op_add(vx, b.i16(i.kk()));
		b.write_array(m.V, i.x(), res);
	}

	void IREmitter::operator()(const SETR &i) {
		//8xy0 - LD Vx, Vy
		//Set Vx = Vy.
		//Stores the value of register Vy in register Vx.
		auto val = b.read_array(m.V, i.y());
		b.write_array(m.V, i.x(), val);
	}

	void IREmitter::operator()(const ORR &i) {
		//8xy1 - OR Vx, Vy
		//Set Vx = Vx OR Vy.
		//
		//Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
		//A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
		// then the same bit in the result is also 1. Otherwise, it is 0.
		auto x = b.read_array(m.V, i.x());
		auto y = b.read_array(m.V, i.y());
		auto res = b.op_or(x, y);
		b.write_array(m.V, i.x(), res);
	}

	void IREmitter::operator()(const ANDR &i) {
		//8xy2 - AND Vx, Vy
		//Set Vx = Vx AND Vy.
		auto x = b.read_array(m.V, i.x());
		auto y = b.read_array(m.V, i.y());
		auto res = b.op_and(x, y);
		b.write_array(m.V, i.x(), res);
	}

	void IREmitter::operator()(const XORR &i) {
		//8xy3 - XOR Vx, Vy
		//Set Vx = Vx XOR Vy.
		auto x = b.read_array(m.V, i.x());
		auto y = b.read_array(m.V, i.y());
		auto res = b.op_xor(x, y);
		b.write_array(m.V, i.x(), res);
	}

	void IREmitter::operator()(const ADDR &i) {
		//8xy4 - ADD Vx, Vy
		//Set Vx = Vx + Vy, set VF = carry.
		//
		//The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
		//Vx and Vy are 8bit registers
		auto x = b.read_array(m.V, i.x());
		auto y = b.read_array(m.V, i.y());
		//auto res = b.op_add(x,y);
		auto[sum, carrybit] = b.uadd_with_overflow(x, y);
		//expand carrybit to byte
		auto carry = b.bldr.CreateSExt(carrybit, IntegerType::getInt8Ty(b.ctx));
		b.write_array(m.V, i.x(), sum);
		b.write_array(m.V, 0xF, carry);
	}

	void IREmitter::operator()(const SUBR &i) {
		//8xy5 - SUB Vx, Vy
		//Set Vx = Vx - Vy, set VF = NOT borrow.
		//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
		//TODO: handle Vf overflow check
		auto x = b.read_array(m.V, i.x());
		auto y = b.read_array(m.V, i.y());
		auto res = b.op_sub(x, y);
		b.write_array(m.V, i.x(), res);
	}

	void IREmitter::operator()(const SUBR_R &i) {
		//8xy7 - SUBN Vx, Vy
		//Set Vx = Vy - Vx, set VF = NOT borrow.
		//If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
		//TODO: handle Vf check
		auto x = b.read_array(m.V, i.x());
		auto y = b.read_array(m.V, i.y());
		auto res = b.op_sub(y, x);
		b.write_array(m.V, i.x(), res);
	}

	void IREmitter::operator()(const SHR &i) {
		//8xy6 - SHR Vx {, Vy}
		//Set Vx = Vx SHR 1.
		//If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
		auto x = b.read_array(m.V, i.x());
		auto res = b.op_and(x, 0x1);
		b.write_array(m.V, 0xF, res);
		auto v = b.bldr.CreateAShr(x, 1);
		b.write_array(m.V, i.x(), v);
	}

	void IREmitter::operator()(const SHL &i) {
		//8xyE - SHL Vx {, Vy}
		//Set Vx = Vx SHL 1.
		//If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
		auto x = b.read_array(m.V, i.x());
		auto res = b.op_and(x, 0x1);
		b.write_array(m.V, 0xF, res);
		auto v = b.bldr.CreateShl(x, 1);
		b.write_array(m.V, i.x(), v);
	}

	void IREmitter::operator()(const SKNER &i) {
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const ISETI &i) {
		//Annn - LD I, addr
		//Set I = nnn.
		//The value of register I is set to nn
		b.bldr.CreateStore(b.i16(i.nnn()), m.I);
	}

	void IREmitter::operator()(const JUMPIND &i) {
		//Bnnn - JP V0, addr
		//Jump to location nnn + V0.
		//The program counter is set to nnn plus the value of V0.
		auto val = b.bldr.CreateLoad(m.I);
		auto res = b.op_add(val, i.nnn());
		b.bldr.CreateStore(res, m.I);

	}

	void IREmitter::operator()(const RAND &i) {
		//Cxkk - RND Vx, byte
		//Set Vx = random byte AND kk.
		//The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
		auto rfunc = b.bldr.CreateCall(m.rand);
		Value *rnum = rfunc;
		auto val = b.op_and(rnum, i.kk());
		b.write_array(m.V, i.x(), val);
	}

	void IREmitter::operator()(const DRAW &i) {
		std::cerr << "DRAW: skipping...\n";
		//throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SKEK &i) {
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SKNEK &i) {
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const GETD &i) {
		//Fx07 - LD Vx, DT
		//Set Vx = delay timer value.
		//The value of DT is placed into Vx.
		auto val = b.bldr.CreateLoad(m.delay);
		b.write_array(m.V, i.x(), val);
	}

	void IREmitter::operator()(const KEY &i) {
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const SETD &i) {
		//Fx15 - LD DT, Vx
		//Set delay timer = Vx.
		//DT is set equal to the value of Vx.
		auto val = b.read_array(m.V, i.x());
		b.bldr.CreateStore(val, m.delay);
	}

	void IREmitter::operator()(const SETS &i) {
		//Fx18 - LD ST, Vx
		//Set sound timer = Vx.
		//ST is set equal to the value of Vx.
		auto val = b.read_array(m.V, i.x());
		b.bldr.CreateStore(val, m.sound);
	}

	void IREmitter::operator()(const IADDR &i) {
		//Fx1E - ADD I, Vx
		//Set I = I + Vx.
		//The values of I and Vx are added, and the results are stored in I.
		auto x = b.bldr.CreateLoad(m.I);
		auto y = b.read_array(m.V, i.x());
		auto res = b.op_add(x, y);
		b.bldr.CreateStore(res, m.I);
	}

	void IREmitter::operator()(const SPRITE &i) {
		//Fx29 - LD F, Vx
		//Set I = location of sprite for digit Vx.
		//The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
		std::cerr << "SPRITE: skipping...\n";
		//throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const BCD &i) {
		//Fx33 - LD B, Vx
		//Store BCD representation of Vx in memory locations I, I+1, and I+2.
		//The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
		throw std::runtime_error(std::string{i.get_mnemonic()} + " instruction not implemented");
	}

	void IREmitter::operator()(const STORE &i) {
		//Fx55 - LD [I], Vx
		//Store registers V0 through Vx in memory starting at location I.
		//The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
		auto itemp = b.bldr.CreateAlloca(IntegerType::getInt16Ty(b.ctx));
		auto i_val = b.bldr.CreateLoad(m.I);
		for (int j = 0; j < i.x() + 1; j++) {
			auto v = b.read_array(m.V, j);
			//auto vext = b.bldr.CreateZExt(v, IntegerType::getInt16Ty(b.ctx));
			b.write_array(m.MEM, i_val, v);
			auto add = b.op_add(i_val, 1);
			b.bldr.CreateStore(add, itemp);
			i_val = b.bldr.CreateLoad(itemp);
		}
	}

	void IREmitter::operator()(const LOAD &i) {
		//Fx65 - LD Vx, [I]
		//Read registers V0 through Vx from memory starting at location I.
		//The interpreter reads values from memory starting at location I into registers V0 through Vx.
		auto itemp = b.bldr.CreateAlloca(IntegerType::getInt16Ty(b.ctx));
		auto i_val = b.bldr.CreateLoad(m.I);
		for (int j = 0; j < i.x(); j++) {
			auto v = b.read_array(m.MEM, i_val);
			//auto vext = b.bldr.CreateZExtOrTrunc(v, IntegerType::getInt8Ty(b.ctx));
			b.write_array(m.V, j, v);

			auto add = b.op_add(i_val, 1);
			b.bldr.CreateStore(add, itemp);
			i_val = b.bldr.CreateLoad(itemp);
		}
	}
}