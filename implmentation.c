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
bool xor8way(u8 x) { return 0; }

// TODO implement MUX4WAY16
// if sel=00 then out=a else if sel=01 then out=b else if sel=10 then out=c else if sel=11 then out=d
u16 mux4way16(u16 a, u16 b, u16 c, u16 d, u8 s) { return 0;}

// TODO implement MUX4WAY16
// if sel=000 then out=a else if sel=001 then out=b else if sel=010 then out=c else if sel=011 then out=d
// else if sel=100 then out=e else if sel=101 then out=f else if sel=110 then out=g else if sel=111 then out=h
u16 mux8way16(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h, u8 s) { return 0; }

// TODO implement DMUX4WAY
// if sel=00 then {a=in, b=c=d=0} else if sel=01 then {b=in, a=c=d=0}
// else if sel=10 then {c=in, a=b=d=0} else if sel=11 then {d=in, a=b=c=0}
// u16 dmux4way(u16 a, u16 b, u16 c, u16 d, u8 s) { return 0 ;}

// TODO implement DMUX8WAY
// if sel=000 then {a=in, *=0} else if sel=001 then {b=in, *=0} 
// else if sel=010 then {c=in, *=0} else if sel=011 then {d=in, *=0}
// else if sel=100 then {e=in, *=0} else if sel=101 then {f=in, *=0}
// else if sel=110 then {g=in, *=0} else if sel=111 then {h=in, *=0}
// u16 dmux8way(u16 a, u16 b, u16 c, u16 d, u8 s) { return 0 ;}