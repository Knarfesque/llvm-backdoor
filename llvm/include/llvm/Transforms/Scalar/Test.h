//===- WarnMissedTransforms.h -----------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Emit warnings if forced code transformations have not been performed.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_TEST_H
#define LLVM_TRANSFORMS_SCALAR_TEST_H

#include "llvm/IR/PassManager.h"

namespace llvm {
class Function;
class Module;

// New pass manager boilerplate.
/*class Test
    : public PassInfoMixin<Test> {
public:
  explicit Test() {}

};*/

// Legacy pass manager boilerplate.
Pass *createTest();
void initializeTestPass(PassRegistry &);
} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_TEST_H_
