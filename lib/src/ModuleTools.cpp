//
// Created by antoshre on 7/16/2020.
//


#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <iostream>
#include <iomanip>
#include "chip8/lifter/ModuleTools.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"

#include "llvm/IR/AssemblyAnnotationWriter.h"
//#include "hacklift/embed.h"
using namespace llvm;

namespace chip8::lifter {

	//TODO: Can all this be static?
	void do_passes(llvm::Module &module, bool optimize = false, bool print_module = false) {


		//Optimization setup based on https://github.com/bollu/simplexhc-cpp/blob/master/src/main.cpp

		PassBuilder PB;

		ModulePassManager MPM;
		FunctionPassManager FPM;
		CGSCCPassManager CGSCCPM;

		if (optimize) {
			PassBuilder::OptimizationLevel optimisationLevel = PassBuilder::OptimizationLevel::O3;
			MPM = PB.buildModuleSimplificationPipeline(optimisationLevel, PassBuilder::ThinLTOPhase::None);
			FPM = PB.buildFunctionSimplificationPipeline(optimisationLevel, PassBuilder::ThinLTOPhase::None);
		}
		LoopAnalysisManager LAM;
		FunctionAnalysisManager FAM;
		CGSCCAnalysisManager CGAM;
		ModuleAnalysisManager MAM;

		// Register the AA manager first so that our version is the one used.
		if (optimize) {
			FAM.registerPass([&] { return PB.buildDefaultAAPipeline(); });
		}

		PB.registerModuleAnalyses(MAM);
		PB.registerCGSCCAnalyses(CGAM);
		PB.registerFunctionAnalyses(FAM);
		PB.registerLoopAnalyses(LAM);
		PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

		// Fix the IR first, then run optimisations.
		MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
		MPM.addPass(createModuleToPostOrderCGSCCPassAdaptor(std::move(CGSCCPM)));

		//Print module IR
		if (print_module) {
			MPM.addPass(PrintModulePass(llvm::outs()));
		}
		// We need to run the pipeline once for correctness. Anything after that
		// is optimisation.
		MPM.run(module, MAM);
	}

	void optimize_module(llvm::Module &module) {
		//Module must be valid for optimization to make sense
		//verifyModule(module, &llvm::outs());
		do_passes(module, true, false);
	}

	void print_module(llvm::Module &module) {
		//do_passes(module, false, true);
		AssemblyAnnotationWriter aaw;
		module.print(llvm::outs(), &aaw);
	}

	//TODO: de-duplicate these run functions.  Dynamically detect signature?  Template and forget?
	void run_void_func(std::unique_ptr<Module> mod, std::unique_ptr<LLVMContext> ctx, const std::string &fname,
	                   std::span<std::uint8_t> mem, bool print_results) {
		InitializeNativeTarget();
		InitializeNativeTargetAsmPrinter();
		InitializeNativeTargetAsmParser();

		// Try to detect the host arch and construct an LLJIT instance.
		auto JIT = orc::LLJITBuilder().create();

		// If we could not construct an instance, return an error.
		if (!JIT) {
			llvm::errs() << JIT.takeError();
			return;
		}

		/*
		auto& DL = JIT->get()->getDataLayout();
		auto& JD = JIT->get()->getMainJITDylib();
		orc::MangleAndInterner Mangle(JD.getExecutionSession(), DL);

		orc::SymbolMap M;
		M[Mangle("handle_keyboard")] = JITEvaluatedSymbol(pointerToJITTargetAddress(&handle_keyboard), JITSymbolFlags());
		M[Mangle("handle_screen")] = JITEvaluatedSymbol(pointerToJITTargetAddress(&handle_screen), JITSymbolFlags());
		cantFail(JD.define(orc::absoluteSymbols(M)));
		*/

		// Add the module.
		if (auto err = JIT->get()->addIRModule(orc::ThreadSafeModule(std::move(mod), std::move(ctx)))) {
			llvm::errs() << err;
			return;
		}
		// Look up the JIT'd code entry point.
		auto sym = JIT->get()->lookup(fname);
		if (!sym) {
			llvm::errs() << sym.takeError();
			return;
		}

		auto f = (void (*)(std::uint8_t *)) sym.get().getAddress();

		if (print_results) {
			std::cout << "Memory before run:\n";
			for (int i = 0; i < 16; i++) {
				std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mem[i] << ' ';
			}
			std::cout << '\n';
		}

		f(mem.data());

		if (print_results) {
			std::cout << "Memory after run:\n";
			for (int i = 0; i < 16; i++) {
				std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mem[i] << ' ';
			}
		}
	}

	void verify_module(llvm::Module &module) {
		if (verifyModule(module, &llvm::outs())) {
			throw std::runtime_error("Module failed to verify.");
		}
	}
}

