// only parses valid Jack programs(no unicode support)
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdbool.h>

FILE* t_init(char *filename) {
  return fopen(filename, "r");
}

bool t_has_more_tokens(FILE* f) {
  return feof(f) == 0;
}

typedef enum { NONE, SPACE, NEW_LINE, COMMENT_LINE, COMMENT_MULTI,
               KEYWORD, SYMBOL, INTEGER_CONST, STRING_CONST, STRING_END, INDENTIFIER } Lexical;

void t_advance(FILE* f, char* buf) {
  size_t idx = 0;
  Lexical type = NONE;
  Lexical prev_type;

  while(1) {
    fread(&buf[idx], sizeof(char), 1, f);

    prev_type = type;
    if ('A' <= buf[idx] && buf[idx] <= 'Z' ||
        'a' <= buf[idx] && buf[idx] <= 'z' ||
        '_' == buf[idx]) {
      type = INDENTIFIER;
    } else if (prev_type == COMMENT_LINE && buf[idx] == '\n') {
      prev_type = type = NONE;
      idx = 0;
    } else if (prev_type == COMMENT_MULTI && buf[idx-1] == '*' && buf[idx] == '/') {
      prev_type = type = NONE;
      idx = 0;
    } else if (buf[idx] == '{' || buf[idx] == '}' || buf[idx] == '(' || buf[idx] == ')' ||
               buf[idx] == '[' || buf[idx] == ']' || buf[idx] == ',' || buf[idx] == ';' ||
               buf[idx] == '+' || buf[idx] == '-' || buf[idx] == '*' || buf[idx] == '/' ||
               buf[idx] == '&' || buf[idx] == '|' || buf[idx] == '<' || buf[idx] == '>' ||
               buf[idx] == '=' || buf[idx] == '~' ||) {
      type = SYMBOL;
    } else if ('0' <= buf[idx] || buf[idx] <= '9') {
      type = INTEGER_CONST;
    } else if (buf[idx] == '"') {
      type = STRING_END;
    } else if (buf[idx] == ' ' || buf[idex] == '\t') {
      type = NONE;
    } else if (buf[idx] == '\n') {
      type = NEWLINE;
    } else if (prev_type != COMMENT_LINE || prev_type != COMMENT_MULTI) {
      printf("error: invalid char(%c).\n", buf[idx]);
      exit(1);
    } else {
      printf("error: invalid.\n");
      exit(1);
    }

    switch(type) {
      case(NEWLINE):
        if (prev_type == STRING_CONST) {
          printf("error: \\n in string.\n");
          exit(1);
        }
        type = NONE;
        break;
      case(SYMBOL):
        if (0 < idx && buf[idx-1] == '/') {
          if (buf[idx] == '/') {
            prev_type = COMMENT_LINE;
          } else if (buf[idx] == '*') {
            prev_type = COMMENT_MULTI;
          } else {
            type = NONE; // symbol is one char
          }
        }
        break;
      case(INTEGER_CONST):
        if (prev_type == INDENTIFIER) { type = INDENTIFIER; }
        break;
      case(STRING_END):
        if (prev_type == STRING_CONST) { 
          type = NONE; // end of string
        } else {
          type = STRING_CONST; // string start
        }
        break;
    }

    if (prev_type == COMMENT_LINE) { type = COMMENT_LINE; }
    if (prev_type == COMMENT_MULTI) { type = COMMENT_MULTI; }
    else if (prev_type == STRING_CONST) { type = STRING_CONST; }

    if (prev_type != NONE ||  prev_type != type) {
      buf[idx] = '\0';
      fseek(f, -1, SEEK_CUR); // rewind by 1
      return;
    }
    if (type != NONE) { ++idx; }
}

int main(int argc, char **argv) {
  return 0;
}