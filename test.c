#include "logic_gate.h"

int main() { return test(); }

// TODO implement NOT gate:
// if in=0 then out=1 else out=0
bool not(bool x) { return 0; }

// TODO implement AND gate:
// if x=y=1 then out=1 else out=0
bool and(bool x, bool y) { return 0; }

// TODO implement OR gate:
// if x=y=0 then out=0 else out=1
bool or(bool x, bool y) { return 0; }

// TODO implement XOR gate:
// if x!=y then out=1 else out=0
bool xor(bool x, bool y) { return 0; }

// TODO implement MUX gate:
// if sel=0 the  out=x else out=y
bool mux(bool x, bool y, bool s) { return 0; }

// TODO implement DMUX gate:
// if sel=0 then {a=in, b=0} else {a=0, b=in}
DMux2 dmux(bool in, bool s) {
  return (DMux2) { 0, 0 };
}

// TODO implement NOT16 gate:
// for i=0..15 out[i]=Not(in[i])
u16 not16(u16 x) { return 0; }

// TODO implement AND16 gate:
// for i=0..15 out[i]=And(in[i])
u16 and16(u16 x, u16 y) { return 0; }

// TODO implement Or16 gate:
// for i=0..15 out[i]=Or(in[i])
u16 or16(u16 x, u16 y) { return 0; }

// TODO implement XOR16 gate:
// for i=0..15 out[i]=Xor(in[i])
u16 xor16(u16 x, u16 y) { return 0; }

// TODO implement MUX16 gate:
// if sel=0 then for i=0..15 out[i]=a[i] else for i=0..15 out[i]=b[i]
u16 mux16(u16 x, u16 y, bool s) { return 0; }

// TODO implement XOR8WAY gate:
// out=Or(in[0],in[1],...,in[7])
bool xor8way(u8 x) { return x; }