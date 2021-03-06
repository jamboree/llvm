//===---------------------- RetireStage.cpp ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file defines the retire stage of an instruction pipeline.
/// The RetireStage represents the process logic that interacts with the
/// simulated RetireControlUnit hardware.
///
//===----------------------------------------------------------------------===//

#include "llvm/MCA/Stages/RetireStage.h"
#include "llvm/MCA/HWEventListener.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "llvm-mca"

namespace llvm {
namespace mca {

llvm::Error RetireStage::cycleStart() {
  if (RCU.isEmpty())
    return llvm::ErrorSuccess();

  const unsigned MaxRetirePerCycle = RCU.getMaxRetirePerCycle();
  unsigned NumRetired = 0;
  while (!RCU.isEmpty()) {
    if (MaxRetirePerCycle != 0 && NumRetired == MaxRetirePerCycle)
      break;
    const RetireControlUnit::RUToken &Current = RCU.getCurrentToken();
    if (!Current.Executed)
      break;
    notifyInstructionRetired(Current.IR);
    RCU.consumeCurrentToken();
    NumRetired++;
  }

  return llvm::ErrorSuccess();
}

llvm::Error RetireStage::execute(InstRef &IR) {
  RCU.onInstructionExecuted(IR.getInstruction()->getRCUTokenID());
  return llvm::ErrorSuccess();
}

void RetireStage::notifyInstructionRetired(const InstRef &IR) const {
  LLVM_DEBUG(llvm::dbgs() << "[E] Instruction Retired: #" << IR << '\n');
  llvm::SmallVector<unsigned, 4> FreedRegs(PRF.getNumRegisterFiles());
  const Instruction &Inst = *IR.getInstruction();

  for (const WriteState &WS : Inst.getDefs())
    PRF.removeRegisterWrite(WS, FreedRegs);
  notifyEvent<HWInstructionEvent>(HWInstructionRetiredEvent(IR, FreedRegs));
}

} // namespace mca
} // namespace llvm
