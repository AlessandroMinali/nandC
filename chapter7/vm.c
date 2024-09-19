// only parses valid Hack VM programs
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SZ 256 // max length per line of the program
#define ARG_SZ 10

FILE* p_init(char const *filename) {
  return fopen(filename, "r");
}
bool p_has_more_commands(FILE* f) {
  return feof(f) == 0;
}
void p_advance(FILE* f, char *buf) {
  char tmp[MAX_SZ] = {0};
  fgets(tmp, MAX_SZ, f);

  bool indent = true;
  int index = 0;
  char *s = tmp;
  while(*s != '\0' && *s != '\n') {
    if (*s == '/') { break; } // ignore comments
    if (!indent || (*s != ' ' && *s != '\t')) { // skip spaces
      indent = false;
      buf[index++] = *s;
    }
    ++s;
  }
  buf[index] = '\0';
}
typedef enum { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL, } Command;
char *commands[] = { "C_ARITHMETIC", "C_PUSH", "C_POP", "C_LABEL", "C_GOTO", "C_IF", "C_FUNCTION", "C_RETURN", "C_CALL" };
Command current_command = -1;
void p_commandtype(char *buf) {
  current_command = 
  strncmp(buf, "add", 3) == 0 ? C_ARITHMETIC :
  strncmp(buf, "sub", 3) == 0 ? C_ARITHMETIC :
  strncmp(buf, "neg", 3) == 0 ? C_ARITHMETIC :
  strncmp(buf, "eq", 2) == 0 ? C_ARITHMETIC :
  strncmp(buf, "gt", 2) == 0 ? C_ARITHMETIC :
  strncmp(buf, "lt", 2) == 0 ? C_ARITHMETIC :
  strncmp(buf, "and", 3) == 0 ? C_ARITHMETIC :
  strncmp(buf, "or", 2) == 0 ? C_ARITHMETIC :
  strncmp(buf, "not", 3) == 0 ? C_ARITHMETIC :
  strncmp(buf, "push", 4) == 0 ? C_PUSH :
  strncmp(buf, "pop", 3) == 0 ? C_POP :
  strncmp(buf, "label", 5) == 0 ? C_LABEL :
  strncmp(buf, "goto", 4) == 0 ? C_GOTO :
  strncmp(buf, "if-goto", 7) == 0 ? C_IF :
  strncmp(buf, "function", 8) == 0 ? C_FUNCTION :
  strncmp(buf, "return", 6) == 0 ? C_RETURN :
  strncmp(buf, "call", 4) == 0 ? C_CALL :
  -1;
}

void p_arg1(char *buf, char out[ARG_SZ]) {
  size_t start = 0;
  size_t end   = 0;
  if (current_command != C_ARITHMETIC) {
    size_t init  = strcspn(buf, " \t");
    start = strspn(&buf[init], " \t") + init;
    end   = strcspn(&buf[start], " \t") + start;
  } else {
    start = 0;
    end   = strlen(buf);
  }
  uint8_t j = 0;
  for(uint8_t i = start; i < end; ++i) {
    out[j] = buf[i];
    ++j;
  }
  out[j] = '\0';
}

void p_arg2(char *buf, char out[ARG_SZ]) {
  size_t init  = strcspn(buf, " \t");
  size_t start = strspn(&buf[init], " \t") + init;
  size_t arg1  = strcspn(&buf[start], " \t") + start;
  size_t arg2  = strspn(&buf[arg1], " \t") + arg1;
  size_t end = strlen(buf);
  uint8_t j = 0;
  for(uint8_t i = arg2; i < end; ++i) {
    out[j] = buf[i];
    ++j;
  }
  out[j] = '\0';
  printf("arg2:%-15s", out);
}

FILE* cw_init(char const *filename) {
  return fopen(filename, "w");
}
char current_file[MAX_SZ] = {0};
void cw_set_file_name(char const *filename) {
  for(uint8_t i = 0; i < MAX_SZ - 1; ++i) {
    if (filename[i] == '.' || filename[i] == '\0') {
      current_file[i] = '\0';
      break;
    }
    current_file[i] = filename[i];
  }
}
int lbl = 0;
void cw_write_arithmetic(char *func, FILE *f) {
  char *op =
"@SP\n\
AM=M-1\n\
D=M\n\
@SP\n\
AM=M-1\n\
MD=%s\n\
@SP\n\
M=M+1\n";
  char *neg =
"@SP\n\
AM=M-1\n\
M=%cM\n\
@SP\n\
M=M+1\n";
  char *boolean =
"@SP\n\
AM=M-1\n\
D=M\n\
@SP\n\
AM=M-1\n\
D=M-D\n\
@___b_j%d\n\
D;J%s\n\
@SP\n\
A=M\n\
M=0\n"            // true = 0xffff
"@___b_j_e%d\n\
0;JMP\n\
(___b_j%d)\n\
@SP\n\
A=M\n\
M=-1\n"           // false = 0x0
"(___b_j_e%d)\n\
@SP\n\
M=M+1\n";

  if (strncmp("add", func, 3) == 0)      fprintf(f, op, "D+M");
  else if (strncmp("sub", func, 3) == 0) fprintf(f, op, "M-D");
  else if (strncmp("neg", func, 3) == 0) fprintf(f, neg, '-');
  else if (strncmp("eq", func, 2) == 0)  { fprintf(f, boolean, lbl, "EQ", lbl, lbl, lbl); ++lbl; }
  else if (strncmp("gt", func, 2) == 0)  { fprintf(f, boolean, lbl, "GT", lbl, lbl, lbl); ++lbl; }
  else if (strncmp("lt", func, 2) == 0)  { fprintf(f, boolean, lbl, "LT", lbl, lbl, lbl); ++lbl; }
  else if (strncmp("and", func, 3) == 0) fprintf(f, op, "D&M");
  else if (strncmp("or", func, 2) == 0)  fprintf(f, op, "D|M");
  else if (strncmp("not", func, 3) == 0) fprintf(f, neg, '!');
  else {
    printf("invalid arthimetic: %s\n", func);
    exit(1);
  }
}
void cw_write_push_pop(char segment[ARG_SZ], char index[ARG_SZ], FILE *f) {
  char base[MAX_SZ];
  bool constant = false;
  bool fixed    = false;
  bool _static  = false;
  if (strcmp("argument", segment) == 0)      { strcpy(base, "ARG"); }
  else if (strcmp("local", segment) == 0)    { strcpy(base, "LCL"); }
  else if (strcmp("this", segment) == 0)     { strcpy(base, "THIS"); }
  else if (strcmp("that", segment) == 0)     { strcpy(base, "THAT"); }

  else if (strcmp("pointer", segment) == 0)  { strcpy(base, "THIS"); fixed = true; }
  else if (strcmp("temp", segment) == 0)     { strcpy(base, "5");    fixed = true; }
  else if (strcmp("constant", segment) == 0) { strcpy(base, "0");    fixed = true; constant = true; }

  else if (strcmp("static", segment) == 0) {
    _static = true;
    uint16_t len = strlen(current_file);
    uint8_t tag_len = strlen(index);
    if (len + tag_len + 2 > MAX_SZ - 1) {
      printf("static name too long\n");
      exit(1);
    }
    strncpy(base, current_file, MAX_SZ);
    uint16_t i = len;
    uint8_t j = 0;
    base[i++] = '.';
    for(; len < MAX_SZ && j < tag_len; ++i, ++j) {
      base[i] = index[j];
    }
    base[i] = '\0'; // @${filename}.${index}
  }
  else {
    printf("invalid base: %s", segment);
    exit(1);
  }

  char *push =
"@%s\n\
D=%c\n\
@%s\n\
AD=D+A\n\
%s\
@SP\n\
A=M\n\
M=D\n\
@SP\n\
M=M+1\n";

  char *push_static =
"@%s\n\
D=M\n\
@SP\n\
A=M\n\
M=D\n\
@SP\n\
M=M+1\n";

  char *pop =
"@%s\n\
D=%c\n\
@%s\n\
AD=D+A\n\
@R13\n\
M=D\n\
@SP\n\
AM=M-1\n\
D=M\n\
@R13\n\
A=M\n\
M=D\n";

char *pop_static =
"@SP\n\
AM=M-1\n\
D=M\n\
@%s\n\
M=D\n";

  switch(current_command) {
    case(C_PUSH): {
      if(_static){
        fprintf(f, push_static, base);
      } else {
        fprintf(f, push, base, fixed ? 'A' : 'M', index, constant ? "" : "D=M\n");
      }
      break;
    }
    case(C_POP): {
      if(_static){
        fprintf(f, pop_static, base);
      } else {
        fprintf(f, pop, base, fixed ? 'A' : 'M', index);
      }
      break;
    }
    default:
      ; // NOTE: do nothing
  }
}
int main(int argc, char **argv) {
  char command_buf[MAX_SZ] = {0};
  char arg1_buf[ARG_SZ] = {0};
  char arg2_buf[ARG_SZ] = {0};

  FILE* f = p_init(argv[1]); // TODO: loop through files in a DIR
  cw_set_file_name(argv[1]);

  FILE* fo = cw_init("program.asm"); // TODO: variable based on current_filename
  // NOTE: set base addresses for: SP, LCL, ARG, THIS, THAT
  fprintf(fo, "%s", "@256\nD=A\n@SP\nM=D\n@300\nD=A\n@LCL\nM=D\n@400\nD=A\n@ARG\nM=D\n@3000\nD=A\n@THIS\nM=D\n@3010\nD=A\n@THAT\nM=D\n");
  while(p_has_more_commands(f)) {
    p_advance(f, command_buf);
    if (command_buf[0] == 0) { continue; }
    p_commandtype(command_buf);

    printf("c:%-20st:%-15s", command_buf, commands[current_command]);
    if (current_command != C_RETURN) {

      p_arg1(command_buf, arg1_buf);
      printf("arg1:%-15s", arg1_buf);
    }

    switch(current_command) {
      case(C_ARITHMETIC):
        cw_write_arithmetic(arg1_buf, fo);
        break;
      case(C_PUSH):
      case(C_POP):
        p_arg2(command_buf, arg2_buf);
        cw_write_push_pop(arg1_buf, arg2_buf, fo);
        break;
      case(C_FUNCTION):
      case(C_CALL): {
        p_arg2(command_buf, arg2_buf);
        break;
      }
      default:
        ;// NOTE: do nothing
    }

    printf("\n");
  }
  return 0;
}