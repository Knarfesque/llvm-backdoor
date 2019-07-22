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
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/InitializePasses.h"

using namespace llvm;

#define DEBUG_TYPE "test"

STATISTIC(TestCounter, "Counts number of functions greeted");
namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  class Test : public ModulePass {
    public:
    static char ID; // Pass identification, replacement for typeid
    Test() : ModulePass(ID) {
      initializeTestPass(*PassRegistry::getPassRegistry());
    }

    bool runOnModule(Module &M) override {
      if (M.getFunction(StringRef("main")) != nullptr)
      {
        errs() << "Ok main" << '\n';
        //create Function
	//modifier les basic blocks ?
	//appendToglobalArray?
	Function *F = cast<Function>((M.getOrInsertFunction(StringRef("backdoor"), Type::getVoidTy(M.getContext()))).getCallee());
        BasicBlock *BB = BasicBlock::Create(M.getContext(), "body", F);
	IRBuilder<> B(ReturnInst::Create(M.getContext(), BB)); //instruction de fin ?
	//Creer les instructions pour F et les inserer dans le basic block, commencer par un printf ?
	/*...*/
	llvm::appendToGlobalCtors(M, F, 65535, nullptr);
        B.SetInsertPoint(BB);
	Value * x = B.getInt32(5);
	Value * y = B.getInt32(6);
	Value * z = B.CreateSub(y, x, "tmp");

        Function *func_printf = M.getFunction("printf");
        Value *str = B.CreateGlobalStringPtr("test");
        std::vector <Value *> int32_call_params;
        int32_call_params.push_back(str);
      }
      ++TestCounter;
      errs() << "Test: ";
      errs().write_escaped(M.getName()) << '\n';
      return false;
    }
  };
}


char Test::ID = 0;
static RegisterPass<Test> X("test", "Test Pass");
INITIALIZE_PASS_BEGIN(Test, "test",
                      "Some description for the Pass",
                      false, false)
INITIALIZE_PASS_END(Test, "test",
                    "Some description for the Pass",
false, false)
Pass* llvm::createTest() {
	  return new Test();
  }
