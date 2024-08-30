#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_def.h"
#include "basic_gate_test.h"
#include "mux_gate_test.h"

void test_dmux_gate() {
  u8 index = 0;
  bool test = 0;

  printf("s | a b | oa ob | p\n");
  printf("-------------------\n");
  for(u8 k = 0; k < 2; ++k) {
    DMux2 dmux_s = { k ? 0 : 1, k ? 1 : 0 };
    bool result = dmux(1, k).a == dmux_s.a && dmux(1, k).b == dmux_s.b;

    printf("%d | %d %d |  %d %d  | %d\n", k, dmux(1, k).a, dmux(1, k).b, dmux_s.a, dmux_s.b, result);
    test |= !result;
    ++index;
  }

  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "DMUX");
}

int test() {
  printf("%s\n\n"KNRM, !not(0) && !not(1) ? KRED"One or more tests failed for NOT." : KGRN"All tests passed for NOT!");
  test_basic_gate(and, nand_output, "AND");
  test_basic_gate(or, nand_output, "OR");
  test_basic_gate(xor, nand_output, "XOR");
  test_mux_gate();
  test_dmux_gate();

  // multi-bit
  u16 x = rand();
  u16 y = rand();

  printf("%s\n\n"KNRM, !not(x) && !not(x) ? KRED"One or more tests failed for NOT16." : KGRN"All tests passed for NOT16!");
  test_basic_gate16(and16, x, y, x&y, "AND16");
  test_basic_gate16(or16, x, y, x&y, "OR16");
  test_basic_gate16(xor16, x, y, x^y, "XOR16");
  test_mux_gate16(x, y);
  test_xor8way();
  return 0;
}
