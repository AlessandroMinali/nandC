void test_mux_gate() {
  u8 index = 0;
  bool test = 0;

  printf("x y s| f o | p\n");
  printf("--------------\n");
  for(u8 k = 0; k < 2; ++k) {
    for(u8 i = 0; i < 2; ++i) {
      for(u8 j = 0; j < 2; ++j) {
        bool mux_s = (k ? i : j) & 1;
        bool result = mux(i, j, k) == mux_s;

        printf("%d %d %d| %d %d | %d\n", i, j, k, mux(i, j, k), mux_s, result);
        test |= !result;
        ++index;
      }
    }
  }
  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "MUX");
}

void test_mux_gate16(u16 x, u16 y) {
  bool test = 0;

  printf("s | f                o                | p\n");
  printf("-----------------------------------------\n");
  for(u8 i = 0; i < 2; ++i){
    bool result = mux16(x, y, i) == (i ? y : x);
    printf("%d | ", i);
    printbits16(mux16(x, y, i));
    printf(" ");
    printbits16(i ? y : x);
    printf(" | ");
    printf("%d \n", result);
    test |= !result;
  }
  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "MUX16");
}