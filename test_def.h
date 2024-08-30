#define u8 uint8_t
#define u16 uint16_t

#define KNRM  "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"

typedef struct DMux2 {
  bool a;
  bool b;
} DMux2;
typedef struct DMux4 {
  bool a;
  bool b;
  bool c;
  bool d;
} DMux4;
typedef struct DMux8 {
  bool a;
  bool b;
  bool c;
  bool d;
  bool e;
  bool g;
  bool h;
} DMux8;

void printbits16(u16 v) {
  for(char i = 15; i >= 0; i--) putchar('0' + ((v >> i) & 1));
}
void printbits8(u8 v) {
  for(char i = 7; i >= 0; i--) putchar('0' + ((v >> i) & 1));
}

// primitive gate
bool nand(bool x, bool y) { return (!(x & y)) & 1; }

// basic gates
bool not(bool x);
bool and(bool x, bool y);
bool or(bool x, bool y);
bool xor(bool x, bool y);

bool mux(bool a, bool b, bool s);
DMux2 dmux(bool in, bool s);

// multi-bit gates
u16 not16(u16 x);
u16 and16(u16 x, u16 y);
u16 or16(u16 x, u16 y);
u16 xor16(u16 x, u16 y);
u16 mux16(u16 a, u16 b, bool s);

// multi-way gates
bool xor8way(u8 x);
u16 mux4way16(u16 a, u16 b, u16 c, u16 d, u8 s);
u16 mux8way16(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h, u8 s);
DMux4 dmux4way(bool in, u8 s);
DMux8 dmux8way(bool in, u8 s);