//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constant.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Transforms/Scalar/Test.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Linker/Linker.h"

#include "llvm/Config/config.h" // for HAVE_LINK_R
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

#define DEBUG_TYPE "test"

STATISTIC(TestCounter, "Counts number of functions greeted");

PreservedAnalyses
TestPass::run(Module &M, FunctionAnalysisManager &AM) {
	return PreservedAnalyses::all();
}

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  class TestLegacy : public ModulePass {
    public:
    static char ID; // Pass identification, replacement for typeid
    explicit TestLegacy() : ModulePass(ID) {
      initializeTestLegacyPass(*PassRegistry::getPassRegistry());
    }

    bool runOnModule(Module &M) override {
      if (M.getFunction(StringRef("main")) != nullptr)
      {
        errs() << "Ok main" << '\n';
        //create Function
	//modifier les basic blocks ?
	//appendToglobalArray?
	std::string ir_file = "/home/alouest/llvm-project/llvm/build/hello.ll";
	llvm::LLVMContext ctx;
	llvm::SMDiagnostic diag;
	std::unique_ptr<Module> mod = llvm::parseIRFile(ir_file, diag, ctx);
	Linker::linkModules(M, std::move(mod));

	Function *F = cast<Function>((M.getOrInsertFunction(StringRef("backdoor"), Type::getVoidTy(M.getContext()))).getCallee());
        BasicBlock *BB = BasicBlock::Create(M.getContext(), "body", F);
	IRBuilder<> B(ReturnInst::Create(M.getContext(), BB)); //instruction de fin ?
	//Creer les instructions pour F et les inserer dans le basic block, commencer par un printf ?
	/*...*/
        B.SetInsertPoint(BB);
	llvm::appendToGlobalCtors(M, F, 65535, nullptr);
	Instruction *newInst = CallInst::Create(F, "tentative");
	Instruction * tst = &(*(BB->begin()));
	BB->getInstList().push_back(newInst);
	
     }
      ++TestCounter;
      errs() << "Test: ";
      errs().write_escaped(M.getName()) << '\n';
      return false;
    }
  };
}


char TestLegacy::ID = 0;
//static RegisterPass<Test> X("test", "Test Pass");
INITIALIZE_PASS_BEGIN(TestLegacy, "test",
                      "Some description for the Pass",
                      false, false)
INITIALIZE_PASS_END(TestLegacy, "test",
                    "Some description for the Pass",
false, false)
Pass* llvm::createTestPass() {
	  return new TestLegacy();
  }
