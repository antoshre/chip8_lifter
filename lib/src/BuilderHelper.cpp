//
// Created by rob on 7/16/2020.
//

#include "chip8/lifter/BuilderHelper.h"

namespace chip8::lifter {
	BuilderHelper::BuilderHelper(IRBuilder<> &b, LLVMContext &c, BlockCache &bc) : bldr(b), ctx(c), bblocks(bc) {}

	void BuilderHelper::jump_if(Value *cond, BasicBlock *target) {
		//Branches require two BB targets: true path and false path
		//Automatically create the false path with a unique name
		std::string false_branch_name = bldr.GetInsertBlock()->getName().str() + "_fallthrough";
		//Check to make sure this name is actually unique in the function
		for (const auto &bb : bldr.GetInsertBlock()->getModule()->getFunction("f")->getBasicBlockList()) {
			if (false_branch_name == bb.getName()) {
				throw std::runtime_error("Fallthrough branch name isn't unique?!");
			}
		}
		auto bb_after_cond = bblocks[false_branch_name];
		bldr.CreateCondBr(cond, target, bb_after_cond);
		//Setup builder to keep inserting after the branch on the false path
		bldr.SetInsertPoint(bb_after_cond);
	}

	Value *BuilderHelper::i8(int val) {
		return llvm::ConstantInt::get(IntegerType::getInt8Ty(ctx), val);
	}

	Value *BuilderHelper::i16(int val) {
		return llvm::ConstantInt::get(IntegerType::getInt16Ty(ctx), val);
	}

	Value *BuilderHelper::write_array(Value *ptr, int offset, int val) {
		Type *t = ptr->getType()->getPointerElementType();
		auto v = llvm::ConstantInt::get(t, val);
		return write_array(ptr, offset, v);
	}

	Value *BuilderHelper::write_array(Value *ptr, int offset, Value *val) {
		Type *t = ptr->getType()->getPointerElementType();
		auto index = llvm::ConstantInt::get(t, offset);
		return write_array(ptr, index, val);
	}

	Value *BuilderHelper::write_array(Value *ptr, Value *offset, Value *val) {
		auto elem = bldr.CreateGEP(ptr, offset);
		return bldr.CreateStore(val, elem);
	}

	Value *BuilderHelper::read_array(Value *ptr, int offset) {
		Type *t = ptr->getType()->getPointerElementType();
		auto index = llvm::ConstantInt::get(t, offset);
		return read_array(ptr, index);

	}

	Value *BuilderHelper::read_array(Value *ptr, Value *offset) {
		auto elem = bldr.CreateGEP(ptr, offset);
		return bldr.CreateLoad(elem);
	}

	//Branch to bb if lhs > rhs
	void BuilderHelper::JGT(Value *lhs, Value *rhs, BasicBlock *bb) {
		auto comp = bldr.CreateICmpSGT(lhs, rhs);
		jump_if(comp, bb);
	}

	void BuilderHelper::JEQ(Value *lhs, Value *rhs, BasicBlock *bb) {
		auto comp = bldr.CreateICmpEQ(lhs, rhs);
		jump_if(comp, bb);
	}

	void BuilderHelper::JGE(Value *lhs, Value *rhs, BasicBlock *bb) {
		auto comp = bldr.CreateICmpSGE(lhs, rhs);
		jump_if(comp, bb);
	}

	void BuilderHelper::JLT(Value *lhs, Value *rhs, BasicBlock *bb) {
		auto comp = bldr.CreateICmpSLT(lhs, rhs);
		jump_if(comp, bb);
	}

	void BuilderHelper::JNE(Value *lhs, Value *rhs, BasicBlock *bb) {
		auto comp = bldr.CreateICmpNE(lhs, rhs);
		jump_if(comp, bb);
	}

	void BuilderHelper::JLE(Value *lhs, Value *rhs, BasicBlock *bb) {
		auto comp = bldr.CreateICmpSLE(lhs, rhs);
		jump_if(comp, bb);
	}

	void BuilderHelper::JMP(Value *lhs, Value *rhs, BasicBlock *bb) {
		JMP(bb);
	}

	void BuilderHelper::JMP(BasicBlock *bb) {
		bldr.CreateBr(bb);
	}

	Value *BuilderHelper::op_add(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return bldr.CreateAdd(lhs, val);
	}

	Value *BuilderHelper::op_add(Value *lhs, Value *rhs) {
		return bldr.CreateAdd(lhs, rhs);
	}

	Value *BuilderHelper::op_sub(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return bldr.CreateSub(lhs, val);
	}

	Value *BuilderHelper::op_sub(Value *lhs, Value *rhs) {
		return bldr.CreateSub(lhs, rhs);
	}

	Value *BuilderHelper::op_not(Value *lhs) {
		return bldr.CreateNot(lhs);
	}

	Value *BuilderHelper::op_neg(Value *lhs) {
		return bldr.CreateNeg(lhs);
	}

	Value *BuilderHelper::op_and(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return bldr.CreateAnd(lhs, val);
	}

	Value *BuilderHelper::op_and(Value *lhs, Value *rhs) {
		return bldr.CreateAnd(lhs, rhs);
	}

	Value *BuilderHelper::op_or(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return bldr.CreateOr(lhs, val);
	}

	Value *BuilderHelper::op_or(Value *lhs, Value *rhs) {
		return bldr.CreateOr(lhs, rhs);
	}

	Value *BuilderHelper::op_xor(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return bldr.CreateXor(lhs, val);
	}

	Value *BuilderHelper::op_xor(Value *lhs, Value *rhs) {
		return bldr.CreateXor(lhs, rhs);
	}

	Value *BuilderHelper::op_mul(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return op_mul(lhs, val);
	}

	Value *BuilderHelper::op_mul(Value *lhs, Value *rhs) {
		return bldr.CreateMul(lhs, rhs);
	}

	std::pair<Value *, Value *> BuilderHelper::uadd_with_overflow(Value *lhs, int rhs) {
		auto val = ConstantInt::get(lhs->getType(), rhs);
		return uadd_with_overflow(lhs, val);
	}

	std::pair<Value *, Value *> BuilderHelper::uadd_with_overflow(Value *lhs, Value *rhs) {
		CallInst *f = bldr.CreateBinaryIntrinsic(Intrinsic::uadd_with_overflow, lhs, rhs);
		Value *results = f;
		return {bldr.CreateExtractValue(results, 0), bldr.CreateExtractValue(results, 1)};
	}

	//Count leading zeroes
	Value *BuilderHelper::leading_zeroes(Value *lhs) {
		auto arg2 = ConstantInt::get(IntegerType::get(ctx, 1), false);
		CallInst *f = bldr.CreateBinaryIntrinsic(Intrinsic::ctlz, lhs, arg2);
		Value *result = f;
		return f;
	}

	std::pair<Value *, Value *> BuilderHelper::usub_with_overflow(Value *lhs, int rhs) {
		return usub_with_overflow(lhs, ConstantInt::get(lhs->getType(), rhs));
	}

	std::pair<Value *, Value *> BuilderHelper::usub_with_overflow(Value *lhs, Value *rhs) {
		CallInst *f = bldr.CreateBinaryIntrinsic(Intrinsic::usub_with_overflow, lhs, rhs);
		Value *results = f;
		return {bldr.CreateExtractValue(results, 0), bldr.CreateExtractValue(results, 1)};
	}


}