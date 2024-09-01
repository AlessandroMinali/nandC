#include "gate.h"

bool nand(bool a, bool b) {
  return !(a&b);
}

bool not(bool a) {
  return nand(a, a);
}

bool and(bool a, bool b) {
  return not(nand(a, b));
}

bool or(bool a, bool b) {
  return not(and(not(a), not(b)));
}

bool xor(bool a, bool b) {
  return or(and(a, not(b)), and(not(a), b));
}

bool mux(bool a, bool b, bool s) {
  return or(and(not(s), a), and(s, b));
}

typedef struct DMUX2 {
  bool a;
  bool b;
} DMUX2;
DMUX2 dmux(a, s) {
  DMUX2 d = { and(a, s), and(a, not(s)) };
  return d;
}

uint16_t not16(a) {
  bool b = false;

  for(uint8_t i = 0; i < 16; ++i) {
    a = gate_bit(a, b, i, not);
  }
  return a;
}

uint16_t and16(uint16_t a, uint16_t b) {
  for(uint8_t i = 0; i < 16; ++i) {
    a = gate_bit(a, b, i, and);
  }
  return a;
}

uint16_t or16(uint16_t a, uint16_t b) {
  for(uint8_t i = 0; i < 16; ++i) {
    a = gate_bit(a, b, i, or);
  }
  return a;
}

uint16_t mux16(uint16_t a, uint16_t b, bool s) {
  SEL = s;
  for(uint8_t i = 0; i < 16; ++i) {
    a = gate_bit(a, b, i, mux);
  }
  return a;
}

int main() {
  printbits(127);
  printbits(23);
  printbits(and16(127, 23));
  return 0;
}