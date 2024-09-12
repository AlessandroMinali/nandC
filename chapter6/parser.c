// only parses valid Hack programs
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SZ 64

static inline void printbits8(uint16_t v) {
  for(char i = 7; i >= 0; i--) putchar('0' + ((v >> i) & 1));
}
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
  while(1) {
    char *s = tmp;
    while(*s != '\0' && *s != '\n') {
      if (*s == '/') { break; } // ignore comments
      if (*s != ' ') { // skip spaces
        buf[index++] = *s;
      }
      ++s;
    }
    buf[index] = 0;
    if (buf[0] == 0) { fgets(tmp, MAX_SZ, f); } else { return; }
  }
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
  for(; i < strlen(buf); ++i) {
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
  for(uint8_t i = 0; i < strlen(buf); ++i) {
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
  return 0xff; // NOTE: error
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

int main(int argc, char const *argv[])
{
  char command_buf[MAX_SZ] = {0};
  char symbol_buf[MAX_SZ] = {0};
  char dest_buf[4] = {0};
  char comp_buf[4] = {0};
  char jump_buf[4] = {0};
  // @value
  // dest=comp;jump

  FILE* fo = fopen("program.hack", "wb");
  FILE* f = p_init(argv[1]);
  while(p_has_more_commands(f)) { // PERF: <FILE> functions already can detect this
    p_advance(f, command_buf); // PERF: better to read all at once
    printf("%s\t\t", command_buf);
    if (p_commandType(command_buf) == A_COMMAND) { // PERF: parse command in one go instead of three functions
      p_symbol(command_buf, symbol_buf);
      printf("symbol: %s\t", symbol_buf); //

      uint16_t word = strtol(symbol_buf, NULL, 10);
      writebits(word & 0x7fff, 16, fo);
    } else if (p_commandType(command_buf) == C_COMMAND) {
      p_dest(command_buf, dest_buf);
      if (strlen(dest_buf) > 0) {
        printf("dest: %s:", dest_buf);
        printbits8(c_dest(dest_buf));
        printf("\t");
      }
      p_comp(command_buf, comp_buf);
      if (strlen(comp_buf) > 0) {
        printf("comp: %s:", comp_buf);
        printbits8(c_comp(comp_buf));
        printf("\t");
      }
      p_jump(command_buf, jump_buf);
      if (strlen(jump_buf) > 0) {
        printf("jump: %s:", jump_buf);
        printbits8(c_jump(jump_buf));
        printf("\t");
      }

      uint16_t word = 0xe000 | (c_comp(comp_buf) << 6) |(c_dest(dest_buf) << 3) | c_jump(jump_buf);
      writebits(word, 16, fo);
    }
    printf("\n");
  }
  return 0;
}
