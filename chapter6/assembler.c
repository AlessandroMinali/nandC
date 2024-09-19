// only parses valid Hack programs
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SZ 256   // max length per line of the program
#define MAX_VAR 1024 // max numbers of variables allowed per program

static inline void writebits(uint16_t v, uint8_t sz, FILE *f) {
  for(char i = sz - 1; i >= 0; i--) putc('0' + ((v >> i) & 1), f);
  putc('\n', f);
}

FILE* p_init(char const *filename) {
  return fopen(filename, "r");
}
bool p_has_more_commands(FILE* f) {
  return feof(f) == 0;
}
void p_advance(FILE* f, char *buf) {
  char tmp[MAX_SZ] = {0};
  fgets(tmp, MAX_SZ, f);

  int index = 0;
  char *s = tmp;
  while(*s != '\0' && *s != '\n') {
    if (*s == '/') { break; } // ignore comments
    if (*s != ' ' && *s != '\t') { // skip spaces
      buf[index++] = *s;
    }
    ++s;
  }
  buf[index] = '\0';
}
typedef enum { A_COMMAND, C_COMMAND, L_COMMAND, } Command;
Command p_commandType(char *buf) {
  switch(buf[0]) {
    case('@'): {
      return A_COMMAND;
      break;
    }
    case('('): {
      return L_COMMAND;
      break;
    }
    default: {
      return C_COMMAND;
    }
  }
}

void p_symbol(char *buf, char out[MAX_SZ]) {
  char *s = buf;
  uint8_t i = 0;
  while(*(++s) != '\0' && *s != ')') {
    out[i++] = *s;
  }
  out[i] = '\0';
}

void p_dest(char *buf, char out[4]) {
  uint8_t i = 0;
  uint8_t end = 0;
  uint8_t len = strlen(buf);
  for(; i < len; ++i) {
    if (buf[i] == '=') { end = i; break; }
  }
  for(i = 0; i < 3 && i < end; ++i) {
    out[i] = buf[i];
  }
  out[i] = '\0';
}

void p_comp(char *buf, char out[4]) {
  uint8_t i = 0;
  uint8_t start = 0;
  uint8_t end = strlen(buf);
  for(; i < end; ++i) {
    if (buf[i] == '=') { start = i + 1; }
    else if (buf[i] == ';') { end = i; break; }
  }
  for(i = 0; i < 3 && i < (end - start); ++i) {
    out[i] = buf[start + i];
  }
  out[i] = '\0';
}

void p_jump(char *buf, char out[4]) {
  uint8_t i = 0;
  uint8_t end = strlen(buf);
  uint8_t start = end;
  for(; i < end; ++i) {
    if (buf[i] == ';') { start = i + 1; break; }
  }
  for(i = 0; i < 3 && i < (end - start); ++i) {
    out[i] = buf[start + i];
  }
  out[i] = '\0';
}

// NOTE: allow any order
uint8_t c_dest(char buf[4]) {
  uint8_t bits = 0;
  uint8_t end = strlen(buf);
  for(uint8_t i = 0; i < end; ++i) {
    switch(buf[i]) {
      case('M'): {
        bits |= 0x1;
        break;
      }
      case('D'): {
        bits |= 0x2;
        break;
      }
      case('A'): {
        bits |= 0x4;
        break;
      }
    }
  }
  return bits;

}
uint8_t c_comp(char buf[4]) {
  uint8_t sz = 3;
  if (strncmp("D&A", buf, sz) == 0) { return 0x0; }
  if (strncmp("D+A", buf, sz) == 0) { return 0x2; }
  if (strncmp("A-D", buf, sz) == 0) { return 0x7; }
  if (strncmp("D", buf, sz) == 0) { return 0xc; }
  if (strncmp("!D", buf, sz) == 0) { return 0xd; }
  if (strncmp("-D", buf, sz) == 0) { return 0xf; }
  if (strncmp("D-A", buf, sz) == 0) { return 0x13; }
  if (strncmp("D|A", buf, sz) == 0) { return 0x15; }
  if (strncmp("D+1", buf, sz) == 0) { return 0x1f; }
  if (strncmp("D-1", buf, sz) == 0) { return 0xe; }
  if (strncmp("0", buf, sz) == 0) { return 0x2a; }
  if (strncmp("A", buf, sz) == 0) { return 0x30; }
  if (strncmp("!A", buf, sz) == 0) { return 0x31; }
  if (strncmp("A-1", buf, sz) == 0) { return 0x32; }
  if (strncmp("-A", buf, sz) == 0) { return 0x33; }
  if (strncmp("-1", buf, sz) == 0) { return 0x3a; }
  if (strncmp("A+1", buf, sz) == 0) { return 0x37; }
  if (strncmp("1", buf, sz) == 0) { return 0x3f; }
  if (strncmp("D&M", buf, sz) == 0) { return 0x40; }
  if (strncmp("D+M", buf, sz) == 0) { return 0x42; }
  if (strncmp("M-D", buf, sz) == 0) { return 0x47; }
  if (strncmp("D-M", buf, sz) == 0) { return 0x53; }
  if (strncmp("D|M", buf, sz) == 0) { return 0x55; }
  if (strncmp("M", buf, sz) == 0) { return 0x70; }
  if (strncmp("!M", buf, sz) == 0) { return 0x71; }
  if (strncmp("M-1", buf, sz) == 0) { return 0x72; }
  if (strncmp("-M", buf, sz) == 0) { return 0x73; }
  if (strncmp("M+1", buf, sz) == 0) { return 0x77; }
  printf("invalid command: %s\n", buf);
  exit(1);
}
uint8_t c_jump(char buf[4]) {
  uint8_t sz = 3;
  if (strncmp("JGT", buf, sz) == 0) { return 0x1; }
  if (strncmp("JEQ", buf, sz) == 0) { return 0x2; }
  if (strncmp("JGE", buf, sz) == 0) { return 0x3; }
  if (strncmp("JLT", buf, sz) == 0) { return 0x4; }
  if (strncmp("JNE", buf, sz) == 0) { return 0x5; }
  if (strncmp("JLE", buf, sz) == 0) { return 0x6; }
  if (strncmp("JMP", buf, sz) == 0) { return 0x7; }
  return 0x0;
}

char symbol_table[MAX_VAR][MAX_SZ] = {{0}};
uint16_t address_table[MAX_VAR] = {0};
uint16_t symbol_p = 0x0;
uint16_t address_p = 0x10;
void s_add_entry(char *symbol, uint16_t address) {
  uint8_t i = 0;
  uint8_t end = strlen(symbol);
  for(; i < MAX_SZ - 1 && i < end; ++i) {
    symbol_table[symbol_p][i] = symbol[i];
  }
  symbol_table[symbol_p][i] = '\0';
  address_table[symbol_p] = address;
  ++symbol_p;
}
// bool s_contains(char *symbol) {} // NOTE: collapsed into following function
uint16_t s_get_address(char *symbol) {
  for(uint16_t i = 0; i < MAX_VAR; ++i) {
    if (strncmp(symbol, symbol_table[i], MAX_SZ) == 0) { return address_table[i]; }
  }
  printf("invalid address: %s\n", symbol);
  exit(1);
}

int main(int argc, char const *argv[])
{
  char command_buf[MAX_SZ] = {0};
  char symbol_buf[MAX_SZ] = {0};
  char dest_buf[4] = {0};
  char comp_buf[4] = {0};
  char jump_buf[4] = {0};

  s_add_entry("SP", 0x0);
  s_add_entry("LCL", 0x1);
  s_add_entry("ARG", 0x2);
  s_add_entry("THIS", 0x3);
  s_add_entry("THAT", 0x4);
  s_add_entry("R0", 0x0);
  s_add_entry("R1", 0x1);
  s_add_entry("R2", 0x2);
  s_add_entry("R3", 0x3);
  s_add_entry("R4", 0x4);
  s_add_entry("R5", 0x5);
  s_add_entry("R6", 0x6);
  s_add_entry("R7", 0x7);
  s_add_entry("R8", 0x8);
  s_add_entry("R9", 0x9);
  s_add_entry("R10", 0xa);
  s_add_entry("R11", 0xb);
  s_add_entry("R12", 0xc);
  s_add_entry("R13", 0xd);
  s_add_entry("R14", 0xe);
  s_add_entry("R15", 0xf);
  s_add_entry("SCREEN", 0x4000);
  s_add_entry("KBD", 0x6000);

  // NOTE: PASS 1
  FILE* f = p_init(argv[1]);
  uint16_t line = 0;
  while(p_has_more_commands(f)) {
    p_advance(f, command_buf);
    if (command_buf[0] == 0) { continue; }

    Command c = p_commandType(command_buf);
    switch(c) {
      case(A_COMMAND): {}
      case(C_COMMAND): {
        ++line;
        break;
      }
      case(L_COMMAND): {
        char *tmp = command_buf;
        uint8_t i = 1;
        while(command_buf[i] != '\0' && command_buf[i] != ')') { ++i; }
        command_buf[i] = '\0';
        s_add_entry(++tmp, line);
        break;
      }
    }
  }

  // NOTE: PASS 2
  rewind(f);
  FILE* fo = fopen("program.hack", "wb");
  while(p_has_more_commands(f)) {
    p_advance(f, command_buf);
    if (command_buf[0] == 0) { continue; }

    Command c = p_commandType(command_buf);
    switch(c) {
      case(A_COMMAND): {
        p_symbol(command_buf, symbol_buf);

        uint16_t word = 0;
        if (symbol_buf[0] > '9') { // replace var
          uint16_t address = s_get_address(symbol_buf);
          if (address < 0xffff) {
            word = address;
          } else {
            s_add_entry(symbol_buf, address_p);
            word = address_p;
            ++address_p;
          }
        } else {
          word = strtol(symbol_buf, NULL, 10);
        }
        writebits(word & 0x7fff, 16, fo);
        break;
      }
      case(C_COMMAND): {
        p_dest(command_buf, dest_buf);
        p_comp(command_buf, comp_buf);
        p_jump(command_buf, jump_buf);

        uint16_t word = 0xe000 | (c_comp(comp_buf) << 6) |(c_dest(dest_buf) << 3) | c_jump(jump_buf);
        writebits(word, 16, fo);
        break;
      }
      case(L_COMMAND): {
        // NOTE: do nothing
      }
    }
  }

  return 0;
}
