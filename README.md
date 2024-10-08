# NAND2C

Following the classic computer building course [nand2tetris](https://www.nand2tetris.org) with a pure software implementation that emulates the gates all the way down to NAND!

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

### Hardware
[gate.h](https://github.com/AlessandroMinali/nandC/blob/master/gate.h): hardware implementation in C  
[hack.c](https://github.com/AlessandroMinali/nandC/blob/master/hack.c): Hack computer which runs binaries  
### Software
[assembler.c](https://github.com/AlessandroMinali/nandC/blob/master/chapter6/assembler.c): Hack ASM -> Hack binary  
[vm.c](https://github.com/AlessandroMinali/nandC/blob/master/vm.c): VM lang -> Hack ASM  
[compiler.c(WIP)]((https://github.com/AlessandroMinali/nandC/blob/master/compiler.c)): Jack lang -> VM lang  
[os.c(WIP)](): provides utilities for Jack lang
