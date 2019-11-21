add_llvm_pass_plugin(Bye Bye.cpp)
if (TARGET intrinsics_gen)
  add_dependencies(obj.Bye intrinsics_gen)
endif()

if (LLVM_LINK_LLVM_DYLIB)
  target_link_libraries(Bye PUBLIC LLVM)
else()
  target_link_libraries(Bye
    PUBLIC
    LLVMSupport
    LLVMCore
    LLVMScalarOpts
    LLVMInstCombine
    LLVMTransformUtils
    LLVMAnalysis
    LLVMipo
    LLVMMC
    LLVMPasses
    LLVMLinker
    LLVMIRReader
    LLVMBitReader
    LLVMMCParser
    LLVMObject
    LLVMTarget
    )
endif()
