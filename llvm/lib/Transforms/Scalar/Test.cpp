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
#include "llvm/Transforms/IPO/FunctionImport.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Transforms/Utils/FunctionImportUtils.h"

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

  static bool isGlobalVarSummary(const ModuleSummaryIndex &Index,
                                 GlobalValue::GUID G) {
    if (const auto &VI = Index.getValueInfo(G)) {
      auto SL = VI.getSummaryList();
      if (!SL.empty())
        return SL[0]->getSummaryKind() == GlobalValueSummary::GlobalVarKind;
    }
    return false;
  }

  static GlobalValue::GUID getGUID(GlobalValue::GUID G) { return G; }

  template <class T>
  static unsigned numGlobalVarSummaries(const ModuleSummaryIndex &Index,
                                       T &Cont) {
    unsigned NumGVS = 0;
    for (auto &V : Cont)
      if (isGlobalVarSummary(Index, getGUID(V)))
        ++NumGVS;
    return NumGVS;
  }

  static void dumpImportListForModule(const ModuleSummaryIndex &Index,
                                      StringRef ModulePath,
                                      FunctionImporter::ImportMapTy &ImportList) {
    for (auto &Src : ImportList) {
      auto SrcModName = Src.first();
      unsigned NumGVSPerMod = numGlobalVarSummaries(Index, Src.second);
      errs() << " - " << Src.second.size() - NumGVSPerMod
                        << " functions imported from " << SrcModName << "\n";
      errs() << " - " << NumGVSPerMod << " vars imported from "
                        << SrcModName << "\n";
    }
  }
 
  void ComputeCrossModuleImportForModuleFromIndex(
       StringRef ModulePath, const ModuleSummaryIndex &Index,
       FunctionImporter::ImportMapTy &ImportList) {
    for (auto &GlobalList : Index) {
      // Ignore entries for undefined references.
      if (GlobalList.second.SummaryList.empty()) //always true right now
        continue;
             
      auto GUID = GlobalList.first;
      assert(GlobalList.second.SummaryList.size() == 1 &&
             "Expected individual combined index to have one summary per GUID");
      auto &Summary = GlobalList.second.SummaryList[0];
      // Skip the summaries for the importing module. These are included to
      // e.g. record required linkage changes.
      if (Summary->modulePath() == ModulePath)
        continue;
      // Add an entry to provoke importing by thinBackend.
      ImportList[Summary->modulePath()].insert(GUID);
    }
  }

  static std::unique_ptr<Module> loadFile(const std::string &FileName,
		                                          LLVMContext &Context) {
      SMDiagnostic Err;
      LLVM_DEBUG(dbgs() << "Loading '" << FileName << "'\n");
      // Metadata isn't loaded until functions are imported, to minimize
      // the memory overhead.
      std::unique_ptr<Module> Result =
          getLazyIRFileModule(FileName, Err, Context,
                              /* ShouldLazyLoadMetadata = */ true);
      if (!Result) {
        Err.print("function-import", errs());
        report_fatal_error("Abort");
      }
                  
      return Result;
  }

  bool runOnModule(Module &M) override {
      if (M.getFunction(StringRef("main")) != nullptr)
      {
        errs() << "Ok main" << '\n';
	FunctionImporter::ImportMapTy ImportList;
	Expected<std::unique_ptr<ModuleSummaryIndex>> IndexPtrOrErr = getModuleSummaryIndexForFile("hello2.bc");
	if (!IndexPtrOrErr) {
	  logAllUnhandledErrors(IndexPtrOrErr.takeError(), errs(),
	                        "Error loading file 'hello.bc': ");
	  return false;
	}
	std::unique_ptr<ModuleSummaryIndex> Index = std::move(*IndexPtrOrErr);
	ComputeCrossModuleImportForModuleFromIndex(M.getModuleIdentifier(), *Index, ImportList);
	Index->dump();
	dumpImportListForModule(*Index, "hello.bc", ImportList);
	for (auto &I : *Index) {
	  for (auto &S : I.second.SummaryList) {
	    if (GlobalValue::isLocalLinkage(S->linkage()))
	      S->setLinkage(GlobalValue::ExternalLinkage);
	  }
	}
	if (renameModuleForThinLTO(M, *Index, nullptr)) {
	  errs() << "Error renaming module\n";
	  return false;
	}
	auto ModuleLoader = [&M](StringRef Identifier) {
	  return loadFile(Identifier, M.getContext());
	};
	FunctionImporter Importer(*Index, ModuleLoader);
        Expected<bool> Result = Importer.importFunctions(M, ImportList);
	if (!Result) {
	  logAllUnhandledErrors(Result.takeError(), errs(),
	                        "Error importing module: ");
	  return false;
	}
        errs() << "ok\n";	
      }
      ++TestCounter;
      errs() << "Test: ";
      errs().write_escaped(M.getName()) << '\n';
      return true;
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
