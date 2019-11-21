#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
using namespace llvm;

namespace {
  struct MyPass : public PassInfoMixin<MyPass> {
      PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
            if(F.hasName())
              errs() << "Hello " << F.getName() << "\n";
          return PreservedAnalyses::all();
            }
  };
} // end anonymous namespace


extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
    return {
          LLVM_PLUGIN_API_VERSION, "MyPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                  [](StringRef Name, FunctionPassManager &FPM,
                         ArrayRef<PassBuilder::PipelineElement>) {
                      if(Name == "mypass"){
                            FPM.addPass(MyPass());
                            return true;
                                }
                                    return false;
                                      }
                                      );
                }
      };
} 
