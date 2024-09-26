// only parses valid Hack VM programs
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_SZ 256 // max length per line of the program incl nul-term
#define MAX_NAME_SZ 128 // max length for label names incl nul-term
#define ARG_SZ 10

// GLOBALS
typedef enum { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL, } Command;

static char current_file[MAX_NAME_SZ] = {0};     // file being processed
static Command current_command = -1;             // command being processed
static char current_function[MAX_NAME_SZ] = {0}; // function being processed

static int lbl = 0; // auto-incremented key for vm impl jmp labels
static int rta = 0; // auto-incremented key for vm impl return-address labels
// GLOBAL end

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
void p_command_type(char *buf) {
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
void p_arg1(char *buf, char *out) {
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
void p_arg2(char *buf, char *out) {
  size_t init  = strcspn(buf, " \t");
  size_t start = strspn(&buf[init], " \t") + init;
  size_t arg1  = strcspn(&buf[start], " \t") + start;
  size_t arg2  = strspn(&buf[arg1], " \t") + arg1;
  size_t end = strcspn(&buf[arg2], " \t") + arg2;
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
void cw_set_file_name(char const *filename) {
  for(uint8_t i = 0; i < MAX_NAME_SZ - 1; ++i) {
    if (filename[i] == '.' || filename[i] == '\0') {
      current_file[i] = '\0';
      break;
    }
    current_file[i] = filename[i];
  }
}
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
    if (len + tag_len + 2 > MAX_NAME_SZ - 1) {
      printf("static name too long\n");
      exit(1);
    }
    strncpy(base, current_file, MAX_NAME_SZ);
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
A=D+A\n\
%s\n\
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
        fprintf(f, push, base, fixed ? 'A' : 'M', index, constant ? "D=A" : "D=M");
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

void cw_write_label(char *label, FILE *f) {
  fprintf(f, "(%s$%s)\n", current_function, label);
}
void cw_write_goto(char *label, FILE *f) {
  fprintf(f, "@%s$%s\n0;JMP\n", current_function, label);
}
void cw_write_if(char *label, FILE *f) {
  fprintf(f, "@SP\nAM=M-1\nD=M\n@%s$%s\nD;JNE\n", current_function, label);
}
void cw_write_call(char *label, char* nargs, FILE *f) {
  fprintf(f, "@%s$ret.%d\n", label, rta);
  fprintf(f, "D=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@SP\nD=M\n@%s\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@%s\n0;JMP\n", strlen(nargs) == 0 ? "0" : nargs, label);
  fprintf(f, "(%s$ret.%d)\n", label, rta);
  rta++;
}
void cw_write_return(FILE *f) {
  fprintf(f, "@LCL\nD=M\n@R13\nM=D\n@5\nD=A\n@R13\nA=M-D\nD=M\n@R14\nM=D\n@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\n@ARG\nD=M+1\n@SP\nM=D\n@R13\nAM=M-1\nD=M\n@THAT\nM=D\n@R13\nAM=M-1\nD=M\n@THIS\nM=D\n@R13\nAM=M-1\nD=M\n@ARG\nM=D\n@R13\nAM=M-1\nD=M\n@LCL\nM=D\n@R14\nA=M\n0;JMP\n");
}
void cw_write_function(char *label, char* nlocals, FILE *f) {
  strncpy(current_function, label, MAX_NAME_SZ - 1);
  fprintf(f, "(%s)\n@%s\nD=A\n@R13\nM=D\n(%s.CLEAR)\n@R13\nDM=M-1\n@%s.CLEAR_END\nD;JLT\n@0\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@%s.CLEAR\n0;JMP\n(%s.CLEAR_END)\n", label, strlen(nlocals) == 0 ? "0" : nlocals, label, label, label, label);
}
void cw_write_init(FILE *f) {
  fprintf(f, "@256\nD=A\n@SP\nM=D\n");
  cw_write_call("Sys.init", "", f);
}

int main(int argc, char **argv) {
  char command_buf[MAX_SZ] = {0};
  char arg1_buf[MAX_NAME_SZ] = {0};
  char arg2_buf[MAX_NAME_SZ] = {0};

  char path[MAX_SZ];
  char fname[MAX_SZ];
  memcpy(path, argv[1], strlen(argv[1])+1);

  FILE* fo = cw_init("program.asm");
  cw_write_init(fo);

  bool once = true;
  DIR *d = opendir(argv[1]);
  while(1) {
    if (d != NULL) {
      struct dirent *item;
      while((item = readdir(d))) {
        size_t sz = strlen(item->d_name);
        if (item->d_type == DT_REG &&
            item->d_name[sz-3] == '.' &&
            item->d_name[sz-2] == 'v' &&
            item->d_name[sz-1] == 'm') {
            memcpy(fname, item->d_name, strlen(item->d_name)+1);
            memcpy(&path[strlen(argv[1])], item->d_name, strlen(item->d_name)+1);
          break;
        }
      }
      if (!item) { break; } // traversed entire dir
    } else if (once) { once = false; }
    else { break; }

    FILE* f = p_init(path);
    cw_set_file_name(fname);
    while(p_has_more_commands(f)) {
      p_advance(f, command_buf);
      if (command_buf[0] == 0) { continue; }
      p_command_type(command_buf);

      if (current_command != C_RETURN) {
        p_arg1(command_buf, arg1_buf);
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
        case(C_LABEL):
          cw_write_label(arg1_buf, fo);
          break;
        case(C_GOTO):
          cw_write_goto(arg1_buf, fo);
          break;
        case(C_IF):
          cw_write_if(arg1_buf, fo);
          break;
        case(C_RETURN):
          cw_write_return(fo);
          break;
        case(C_FUNCTION):
          p_arg2(command_buf, arg2_buf);
          cw_write_function(arg1_buf, arg2_buf, fo);
          break;
        case(C_CALL): {
          p_arg2(command_buf, arg2_buf);
          cw_write_call(arg1_buf, arg2_buf, fo);
          break;
        }
        default:
          ;// NOTE: do nothing
      }
    }
  }
  return 0;
}