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
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

#define DEBUG_TYPE "test"

STATISTIC(TestCounter, "Counts number of functions greeted");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct Test : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    Test() : ModulePass(ID) {}

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
        CallInst * int32_call = CallInst::Create(func_printf, int32_call_params, "call", b);
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
