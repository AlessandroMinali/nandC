// TODO: keyboard in, screen output via SDL

#include <unistd.h>
#include <stdlib.h>
#include "gate.h"

static char buf[17] = {0};
void load_program(char *filename, RAM16K *instructions) {
  FILE *f = fopen(filename, "r");
  char buf[18] = {0};
  int i = 0;
  while(fgets(buf, 18, f) && i < 0x4000) {
    ram16k(strtol(buf, NULL, 2), true, i, instructions);
    ++i;
  }
}
bool print16(REG16 r) {
  bool out = false;
  for(int i = 15; i >= 0; i--) {
    if (r.d[i].next) {
      buf[15 - i] = '1';
      out = true;
    } else { 
      buf[15 - i] = '0';
    }
  }
  return out;
}
void printreg16(REG16 d, char *name) {
  print16(d);
  printf("%2s: %-10ld", name, strtol(buf, NULL, 2));
}
void printram16k(RAM16K d) {
  for(uint8_t i = 0; i < 4; ++i) {
    for(uint8_t j = 0; j < 8; ++j) {
      for(uint8_t k = 0; k < 8; ++k) {
        for(uint8_t l = 0; l < 8; ++l) {
          for(uint8_t m = 0; m < 8; ++m) {
            if (print16(d.d[i].d[j].d[k].d[l].d[m])) {
              printf("0x%04x: %-10ld", i*4096+j*512+k*64+l*8+m, strtol(buf, NULL, 2));
            }
          }
        }
      }
    }
  }
}
void printcomputer(RAM16K data, RAM16K screen, REG16 keyboard, REG16 a, REG16 d, REG16 pc) {
  printf("\e[1;1H\e[2J");
  printreg16(pc, "PC");
  printreg16(a, "A");
  printreg16(d, "D");
  printf("\nDATA\n");
  printram16k(data);
  printf("\n\nSCREEN\n");
  printram16k(screen);
  printf("\n");
}

int main(int argc, char **argv) {
  RAM16K instruction = {0};
  load_program(argv[1], &instruction);

  RAM16K data = {0};
  RAM16K screen = {0};
  REG16 keyboard = {0};
  REG16 a = {0};
  REG16 d = {0};
  REG16 _pc = {0};

  while(1) {
    computer(0, &instruction, &data, &screen, &keyboard, &a, &d, &_pc);
    printcomputer(data, screen, keyboard, a, d, _pc);
    usleep(100000);
  }

  return 0;
}