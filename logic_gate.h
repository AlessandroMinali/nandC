#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_def.h"
#include "basic_gate_test.h"
#include "mux_gate_test.h"

int test() {
  printf("%s\n\n"KNRM, !not(0) && !not(1) ? KRED"One or more tests failed for NOT." : KGRN"All tests passed for NOT!");
  test_basic_gate(and, and_output, "AND");
  test_basic_gate(or, or_output, "OR");
  test_basic_gate(xor, xor_output, "XOR");
  test_mux_gate();
  test_dmux_gate();

  // multi-bit
  u16 x = rand();
  u16 y = rand();
  u16 a = rand();
  u16 b = rand();
  u16 c = rand();
  u16 d = rand();
  u16 e = rand();
  u16 f = rand();
  u16 g = rand();
  u16 h = rand();

  printf("%s\n\n"KNRM, !(not16(x) == !x) ? KRED"One or more tests failed for NOT16." : KGRN"All tests passed for NOT16!");
  test_basic_gate16(and16, x, y, x&y, "AND16");
  test_basic_gate16(or16, x, y, x|y, "OR16");
  test_basic_gate16(xor16, x, y, x^y, "XOR16");
  test_mux_gate16(x, y);
  test_xor8way();
  test_mux4_gate16(a, b, c, d);
  test_mux8_gate16(a, b, c, d, e, f, g, h);
  // test_dmux4_gate();
  // test_dmux8_gate();
  return 0;
}
