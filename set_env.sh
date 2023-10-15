#!/bin/bash

LLVM_DIR="$1"
export LLVM_DIR
alias clang-16="$LLVM_DIR/bin/clang"
alias opt-16="$LLVM_DIR/bin/opt"
alias lli-16="$LLVM_DIR/bin/lli"
