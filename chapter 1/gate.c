#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* HELPER FUNCTION */
// print 16-bit binary number
inline void printbits(uint16_t v) {
  for(char i = 15; i >= 0; i--) putchar('0' + ((v >> i) & 1));
    putchar('\n');
}

// get bit <pos> of <a>
inline bool get_bit(uint16_t a, uint8_t pos) {
  return (a >> pos) & 0x1;
}
// set bit <pos> of <a> to value <v>
inline uint16_t set_bit(uint16_t a, bool v, uint8_t pos) {
  return (a & ~(0x1 << pos)) | (v << pos);
}
// apply <gate> to bit <pos> of <a>, <b>
inline uint16_t gate_bit(uint16_t a, uint16_t b, uint8_t pos, bool (*gate)(bool, bool)) {
  return set_bit(a, gate(get_bit(a, pos), get_bit(b, pos)), pos);
}

// NAND fundamental gate
bool nand(bool a, bool b) {
  return !(a&b);
}
/* END OF HELPER FUNCTIONS */

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
  bool a; bool b;
} DMUX2;
DMUX2 dmux(bool a, bool s) {
  DMUX2 d = { and(a, not(s)), and(a, s) };
  return d;
}

uint16_t not16(a) {
  for(uint8_t i = 0; i < 16; ++i) {
    a = set_bit(a, not(get_bit(a, i)), i);
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
  for(uint8_t i = 0; i < 16; ++i) {
    a = set_bit(a, mux(get_bit(a, i), get_bit(b, i), s), i);
  }
  return a;
}

bool or8way(uint8_t a) {
  bool b = false;

  for(uint8_t i = 0; i < 8; ++i) {
    b = gate_bit(a, b, i, or);
  }
  return b;
}

uint16_t mux4way16(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint8_t s2) {
  return mux16(
           mux16(a, b, get_bit(s2, 0)),
           mux16(c, d, get_bit(s2, 0)),
         get_bit(s2, 1));
}

uint16_t mux8way16(uint16_t a, uint16_t b, uint16_t c, uint16_t d,
                   uint16_t e, uint16_t f, uint16_t g, uint16_t h, uint8_t s3) {
  return mux16(
           mux4way16(a, b, c, d, s3),
           mux4way16(e, f, g, h, s3),
         get_bit(s3, 2));
}

typedef struct DMUX4 {
  bool a; bool b; bool c; bool d;
} DMUX4;
DMUX4 dmux4way(a, s2) {
  DMUX2 in   = dmux(a, get_bit(s2, 1));
  DMUX2 out1 = dmux(in.a, get_bit(s2, 0));
  DMUX2 out2 = dmux(in.b, get_bit(s2, 0));
  DMUX4 d    = { out1.a, out1.b, out2.a, out2.b };

  return d;
}

typedef struct DMUX8 {
  bool a;  bool b; bool c; bool d;
  bool e;  bool f; bool g; bool h;
} DMUX8;
DMUX8 dmux8way(a, s3) {
  DMUX2 in   = dmux(a, get_bit(s3, 2));
  DMUX4 out1 = dmux4way(in.a, s3);
  DMUX4 out2 = dmux4way(in.b, s3);
  DMUX8 d    = { out1.a, out1.b, out1.c, out1.d,
                 out2.a, out2.b, out2.c, out2.d };

  return d;
}

// TODO: halfadder, fulladder, add16, inc16, ALU
// TODO: dff, bit, register, ram8, ram64, ram512, ram4k, ram16k, PC
// TODO: memory, CPU, computer, screen, keyboard, dregister, aregister, rom32k, ram16k

int main() {
  //   printbits(mux8way16(0xf, 0xf0, 0xf00, 0xf000, 0x1, 0xf1, 0xf01, 0xf001, i));
  // }

  for(char i = 0; i < 8; ++i) {
    printbits(dmux8way(1, i).a);
    printbits(dmux8way(1, i).b);
    printbits(dmux8way(1, i).c);
    printbits(dmux8way(1, i).d);
    printbits(dmux8way(1, i).e);
    printbits(dmux8way(1, i).f);
    printbits(dmux8way(1, i).g);
    printbits(dmux8way(1, i).h);
    printf("\n");
  }
  return 0;
}