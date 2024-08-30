bool nand_output[4] = { 1, 1, 1, 0 };
bool and_output[4] = { 0, 0, 0, 1 };
bool or_output[4] = { 0, 1, 1, 1 };
bool xor_output[4] = { 0, 1, 1, 0 };

void test_basic_gate(bool (*f)(bool, bool), bool out[4], char *gate) {
  u8 index = 0;
  bool test = 0;

  printf("x y | f o | p\n");
  printf("-------------\n");
  for(u8 i = 0; i < 2; ++i){
    for(u8 j = 0; j < 2; ++j){
      bool result = f(i, j) == out[index];

      printf("%d %d | %d %d | %d\n", i, j, f(i, j), out[index], result);
      test |= !result;
      ++index;
    }
  }
  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), gate);
}

void test_basic_gate16(u16 (*f)(u16, u16), u16 x, u16 y, u16 out, char *gate) {
  printf("x                y                | f                o                | p\n");
  printf("-------------------------------------------------------------------------\n");
  bool result = f(x, y) == out;

  printbits16(x);
  printf(" ");
  printbits16(y);
  printf(" | ");
  printbits16(f(x, y));
  printf(" ");
  printbits16(out);
  printf(" | ");
  printf("%d \n", result);
  printf("%s %s!\n\n"KNRM, (!result ? KRED"One or more tests failed for" : KGRN"All tests passed for"), gate);
}

void test_xor8way() {
  bool test = 0;

  printf("x        | f o | p\n");
  printf("------------------\n");

  u8 x = rand();
  bool result = xor8way(x) == x;
  test |= !result;
  printbits8(x);
  printf(" | %d %d | %d\n", xor8way(x), (bool)x, result);

  x = 0;
  result = xor8way(x) == x;
  test |= !result;
  printbits8(x);
  printf(" | %d %d | %d\n", xor8way(x), x, result);

  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "XOR8WAY");
}