# NAND2C

Following the classic computer building course [nand2tetris](https://www.nand2tetris.org) with pure software implementation that emulates the gates all the way down to NAND!

Everything is built on these two "gates" implemented in C:
```c
bool nand(bool a, bool b) {
  return !(a&b);
}

typedef struct DFF {
  bool state;
  bool next;
} DFF;
bool dff(bool a, DFF* d) {
  bool out = d->state;
  d->state = d->next;
  d->next = a;
  return out;
}
```
All other gates and components are fuction compositions of the above!

Check out [gate.h](https://github.com/AlessandroMinali/nandC/blob/master/gate.h) to see the chips.  
Check out [hack.c](https://github.com/AlessandroMinali/nandC/blob/master/hack.c) to see the final Hack computer.  
There is a [hack assembler written in C](https://github.com/AlessandroMinali/nandC/blob/master/chapter6/assembler.c) which can be used to generate `.hack` files for consumption by `hack.c`
