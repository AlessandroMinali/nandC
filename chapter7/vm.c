// only parses valid Hack VM programs
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define STACK_SZ 256
#define MAX_SZ 64
#define ARG_SZ 10

// STACK defintion
// size_t _sp = 0; // NOTE: will wrap on overflow
// uint16_t _stack[STACK_SZ] = {0};
// void _push(uint16_t x) { _stack[_sp++] = x; }
// uint16_t _pop (uint16_t x) { return _stack[--_sp]; }
// STACK definition END

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
}

FILE* cw_init(char const *filename) {
  return fopen(filename, "w");
}
// void cw_set_file_name(char const *filename) { } // NOTE: useless?
void cw_write_arithmetic(char arg[ARG_SZ], FILE *f) {
  fputs(arg, f);
  fputs("\n", f);
}
void cw_write_push_pop(char *buf, char segment[ARG_SZ], char index[ARG_SZ], FILE *f) {
  char *base;
  bool constant = false;
  bool fixed = false;
  if (strcmp("argument", segment) == 0)      { base = "ARG"; }
  else if (strcmp("local", segment) == 0)    { base = "LCL"; }
  else if (strcmp("this", segment) == 0)     { base = "THIS"; }
  else if (strcmp("that", segment) == 0)     { base = "THAT"; }

  else if (strcmp("pointer", segment) == 0)  { base = "THIS"; fixed = true; }
  else if (strcmp("temp", segment) == 0)     { base = "5"; fixed = true; }
  else if (strcmp("constant", segment) == 0) { base = "0"; constant = true; }

  // else if (strcmp("static", segment) == 0) { base = argv[1]; }
  // TODO: push static = @Xxx.3; D=M; then push D onto stack
  else {
    printf("invalid base");
    exit(1);
  }

// @R13 = stack + sp
// @R14 = seg + i
  char *stack_pos =
"@256\n\
D=A\n\
@SP\n\
D=D+M\n\
@R13\n\
M=D\n";

  char *seg_index =
"@%s\n\
D=%s\n\
@%s\n\
D=D+A\n\
@R14\n\
M=D\n";

  char *push =
"@R14\n\
%s\
D=M\n\
@R13\n\
A=M\n\
M=D\n";

  char *pop =
"@R13\n\
A=M\n\
D=M\n\
@R14\n\
A=M\n\
M=D\n";

  char *stack_inc =
"@SP\n\
M=M+1\n";
  char *stack_dec =
"@SP\n\
M=M-1\n";

  fprintf(f, seg_index, base, fixed ? "A" : "M", index);
  switch(current_command) {
    case(C_PUSH): {
      fprintf(f, "%s", stack_pos);
      fprintf(f, push, constant ? "": "A=M\n");
      fprintf(f, "%s", stack_inc);
      break;
    }
    case(C_POP): {
      fprintf(f, "%s%s%s", stack_dec, stack_pos, constant ? "" : pop);
      // NOTE: fall-through
    }
    default:
      ; // NOTE: do nothing
  }
}
int main(int argc, char **argv) {
  char command_buf[MAX_SZ] = {0};
  char arg1_buf[ARG_SZ] = {0};
  char arg2_buf[ARG_SZ] = {0};

  FILE* f = p_init(argv[1]);
  FILE* fo = cw_init("program.asm");
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
      case(C_PUSH):
      case(C_POP):
        p_arg2(command_buf, arg2_buf);
        cw_write_push_pop(command_buf, arg1_buf, arg2_buf, fo);
        break;
      case(C_FUNCTION):
      case(C_CALL): {
        p_arg2(command_buf, arg2_buf);
        // NOTE: fall-through
      }
      default:
        ;// NOTE: do nothing
    }
    printf("\n");
  }
  return 0;
}