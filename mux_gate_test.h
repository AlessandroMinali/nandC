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
    printbits(mux16(x, y, i), 16);
    printf(" ");
    printbits(i ? y : x, 16);
    printf(" | ");
    printf("%d \n", result);
    test |= !result;
  }
  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "MUX16");
}

void test_mux4_gate16(u16 a, u16 b, u16 c, u16 d) {
  bool test = 0;
  u16 array[4] = { a, b, c, d};
  
  printf(" s | f                o                | p\n");
  printf("------------------------------------------\n");
  for(u8 i = 0; i < 4; ++i) {
    bool result = mux4way16(a, b, c, d, i) == array[i];
    printbits(i, 2);
    printf(" | ");
    printbits(mux4way16(a, b, c, d, i), 16);
    printf(" ");
    printbits(array[i], 16);
    printf(" | ");
    printf("%d \n", result);
    test |= !result;
  }
  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "MUX4WAY16");
}

void test_mux8_gate16(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h) {
  bool test = 0;
  u16 array[8] = { a, b, c, d, e, f, g, h};
  
  printf("  s | f                o                | p\n");
  printf("-------------------------------------------\n");
  for(u8 i = 0; i < 8; ++i) {
    bool result = mux8way16(a, b, c, d, e, f, g, h, i) == array[i];
    printbits(i, 3);
    printf(" | ");
    printbits(mux8way16(a, b, c, d, e, f, g, h, i), 16);
    printf(" ");
    printbits(array[i], 16);
    printf(" | ");
    printf("%d \n", result);
    test |= !result;
  }
  printf("%s %s!\n\n"KNRM, (test ? KRED"One or more tests failed for" : KGRN"All tests passed for"), "MUX8WAY16");
}

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
 
// TODO:Alessandro test for DMUX4WAY and DMUX8WAY