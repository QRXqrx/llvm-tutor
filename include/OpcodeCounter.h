//==============================================================================
// FILE:
//    OpcodeCounter.h
//
// DESCRIPTION:
//    Declares the OpcodeCounter Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_OPCODECOUNTER_H
#define LLVM_TUTOR_OPCODECOUNTER_H // @Adian: Don't know what is this for?

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
using ResultOpcodeCounter = llvm::StringMap<unsigned>;

// @Adian: Seems "struct == public class" in C++.
struct OpcodeCounter : public llvm::AnalysisInfoMixin<OpcodeCounter> {
  // @Adian: Use using to set type alias. Equivalent to a quick definition of a Java bean/record.
  using Result = ResultOpcodeCounter;
  Result run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &);

  OpcodeCounter::Result generateOpcodeMap(llvm::Function &F);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<OpcodeCounter>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
// @Adian: Declare the class in header file -- Class prototype?
// @Adian: This class is not a struct. So it is likely not public.
class OpcodeCounterPrinter : public llvm::PassInfoMixin<OpcodeCounterPrinter> {
public:
  explicit OpcodeCounterPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  llvm::PreservedAnalyses run(llvm::Function &Func,
                              llvm::FunctionAnalysisManager &FAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};
#endif
