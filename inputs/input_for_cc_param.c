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
#include <stdlib.h>

void foo() { }
void bar() {foo(); }
void fez() {bar(); }
void log(int a) {
  printf("Find param: %d\n", a);
}

int main(int argc, char **argv) {

  if (argc < 1) {
    printf("<input_for_cc_param: <param>\n");
    return 0;
  }

  int a = atoi(argv[0]);
  log(a);

  foo();
  bar();
  fez();

  int ii = 0;
  for (ii = 0; ii < 10; ii++)
    foo();

  return 0;
}
