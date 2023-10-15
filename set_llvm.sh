#!/bin/bash

if [ $# -lt 2 ];then
  echo "<set_llvm>: <llvm_ver> <llvm_build_dir>"
fi

LLVM_VER="$1"
LLVM_DIR="$2"
export LLVM_DIR
alias clang-"$LLVM_VER"="$LLVM_DIR/bin/clang"
alias opt-"$LLVM_VER"="$LLVM_DIR/bin/opt"
alias lli-"$LLVM_VER"="$LLVM_DIR/bin/lli"
