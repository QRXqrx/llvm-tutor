//=============================================================================
// FILE:
//      input_for_cc.c
//
// DESCRIPTION:
//      Sample input file for CallCounter analysis.
//
// License: MIT
//
// @Adian: How the instrumentor handle program need params?
//=============================================================================
#include <stdio.h>

void foo() { }
void bar() {foo(); }
void fez() {bar(); }

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("<input_for_cc_param: <param>\n");
    return 0;
  }

  printf("argv[0]=%s,argv[1]=%s\n", argv[0], argv[1]);

  foo();
  bar();
  fez();

  int ii = 0;
  for (ii = 0; ii < 10; ii++)
    foo();

  return 0;
}
