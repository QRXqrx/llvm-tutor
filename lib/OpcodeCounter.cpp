//=============================================================================
// FILE:
//    OpcodeCounter.cpp
//
// DESCRIPTION:
//    Visits all instructions in a function and counts how many times every
//    LLVM IR opcode was used. Prints the output to stderr.
//
//    This example demonstrates how to insert your pass at one of the
//    predefined extension points, e.g. before any other transformations are
//    run (i.e. via `EP_EarlyAsPossible` for the Legacy PM) or whenever the
//    vectoriser is run (i.e. via `registerVectorizerStartEPCallback` for the
//    new PM).
//
// USAGE:
//    1. New PM
//      opt -load-pass-plugin libOpcodeCounter.dylib `\`
//        -passes="print<opcode-counter>" `\`
//        -disable-output <input-llvm-file>
//    2. Automatically through an optimisation pipeline - new PM
//      opt -load-pass-plugin libOpcodeCounter.dylib --passes='default<O1>' `\`
//        -disable-output <input-llvm-file>
//
// License: MIT
//=============================================================================
#include "OpcodeCounter.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

// @Adian: namespace is somehow like the import in Python.
using namespace llvm;

// Pretty-prints the result of this analysis
// @Adian: Global method; Util method in OO programming.
static void printOpcodeCounterResult(llvm::raw_ostream &,
                              const ResultOpcodeCounter &OC);

//-----------------------------------------------------------------------------
// OpcodeCounter implementation
//-----------------------------------------------------------------------------
// @Adian: `key` is a private field. Why put this field here?
llvm::AnalysisKey OpcodeCounter::Key;

// @Adian: Implement the instance methods (maybe public, no accessor) declared in the head file.
OpcodeCounter::Result OpcodeCounter::generateOpcodeMap(llvm::Function &Func) {
  OpcodeCounter::Result OpcodeMap;

  // @Adian: Traverse every BB in functions and collect the opcode name of the instructions.
  for (auto &BB : Func) {
    for (auto &Inst : BB) {
      StringRef Name = Inst.getOpcodeName();

      if (OpcodeMap.find(Name) == OpcodeMap.end()) {
        OpcodeMap[Inst.getOpcodeName()] = 1;
      } else {
        OpcodeMap[Inst.getOpcodeName()]++;
      }
    }
  }

  return OpcodeMap;
}

// @Adian: Seems run() is inherited from the super class. The true type of OpcodeCounter::Result is llvm::StringMap<unsigned>.
OpcodeCounter::Result OpcodeCounter::run(llvm::Function &Func,
                                         llvm::FunctionAnalysisManager &) {
  return generateOpcodeMap(Func);
}

// @Adian: Printer's run(). The return type is PreservedAnalyses. So Printer is the true Pass.
PreservedAnalyses OpcodeCounterPrinter::run(Function &Func,
                                            FunctionAnalysisManager &FAM) {
  auto &OpcodeMap = FAM.getResult<OpcodeCounter>(Func);

  // In the legacy PM, the following string is printed automatically by the
  // pass manager. For the sake of consistency, we're adding this here so that
  // it's also printed when using the new PM.
  OS << "Printing analysis 'OpcodeCounter Pass' for function '"
     << Func.getName() << "':\n";

  printOpcodeCounterResult(OS, OpcodeMap);
  return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
// @Adian: For New PM, getXXXPluginInfo is needed for every pass class XXX.
llvm::PassPluginLibraryInfo getOpcodeCounterPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "OpcodeCounter", LLVM_VERSION_STRING,
        // @Adian: Seems anonymous class
        [](PassBuilder &PB) {
          // #1 REGISTRATION FOR "opt -passes=print<opcode-counter>"
          // Register OpcodeCounterPrinter so that it can be used when
          // specifying pass pipelines with `-passes=`.
          PB.registerPipelineParsingCallback(
              [&](StringRef Name, FunctionPassManager &FPM,
                  ArrayRef<PassBuilder::PipelineElement>) {
                if (Name == "print<opcode-counter>") {
                  FPM.addPass(OpcodeCounterPrinter(llvm::errs()));
                  return true;
                }
                return false;
              });
          // #2 REGISTRATION FOR "-O{1|2|3|s}"
          // Register OpcodeCounterPrinter as a step of an existing pipeline.
          // The insertion point is specified by using the
          // 'registerVectorizerStartEPCallback' callback. To be more precise,
          // using this callback means that OpcodeCounterPrinter will be called
          // whenever the vectoriser is used (i.e. when using '-O{1|2|3|s}'.
          PB.registerVectorizerStartEPCallback(
              [](llvm::FunctionPassManager &PM,
                 llvm::OptimizationLevel Level) {
                PM.addPass(OpcodeCounterPrinter(llvm::errs()));
              });
          // #3 REGISTRATION FOR "FAM.getResult<OpcodeCounter>(Func)"
          // Register OpcodeCounter as an analysis pass. This is required so that
          // OpcodeCounterPrinter (or any other pass) can request the results
          // of OpcodeCounter.
          PB.registerAnalysisRegistrationCallback(
              [](FunctionAnalysisManager &FAM) {
                FAM.registerPass([&] { return OpcodeCounter(); });
              });
          }
        };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getOpcodeCounterPluginInfo();
}

//------------------------------------------------------------------------------
// Helper functions - implementation
//------------------------------------------------------------------------------
static void printOpcodeCounterResult(raw_ostream &OutS,
                                     const ResultOpcodeCounter &OpcodeMap) {
  OutS << "================================================="
               << "\n";
  OutS << "LLVM-TUTOR: OpcodeCounter results\n";
  OutS << "=================================================\n";
  const char *str1 = "OPCODE";
  const char *str2 = "#TIMES USED";
  OutS << format("%-20s %-10s\n", str1, str2);
  OutS << "-------------------------------------------------"
               << "\n";
  for (auto &Inst : OpcodeMap) {
    OutS << format("%-20s %-10lu\n", Inst.first().str().c_str(),
                           Inst.second);
  }
  OutS << "-------------------------------------------------"
               << "\n\n";
}
