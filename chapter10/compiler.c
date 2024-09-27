// only parses valid Jack programs(no unicode support)
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SZ 512
#define COUNT 500

FILE* t_init(char *filename) {
  return fopen(filename, "r");
}

bool t_has_more_tokens(FILE* f) {
  return feof(f) == 0;
}

typedef enum { NONE, COMMENT_LINE, COMMENT_MULTI,
               KEYWORD, SYMBOL, INTEGER_CONST, STRING, STRING_CONST, INDENTIFIER } Lexical;
char *lex[] = { "NONE", "COMMENT_LINE", "COMMENT_MULTI",
               "KEYWORD", "SYMBOL", "INTEGER_CONST", "STRING", "STRING_CONST", "INDENTIFIER" }; // TODO: remove eventually
Lexical prev_type;
void t_advance(FILE* f, char* buf) {
  size_t idx = 0;
  Lexical type = NONE;

  while(fread(&buf[idx], sizeof(char), 1, f)) {
    prev_type = type;
    if (('A' <= buf[idx] && buf[idx] <= 'Z') ||
        ('a' <= buf[idx] && buf[idx] <= 'z') ||
        '_' == buf[idx]) {
      type = INDENTIFIER;
    } else if (prev_type == COMMENT_LINE && buf[idx] == '\n') {
      prev_type = type = NONE;
      idx = 0;
      continue;
    } else if (prev_type == COMMENT_MULTI && buf[idx-1] == '*' && buf[idx] == '/') {
      prev_type = type = NONE;
      idx = 0;
      continue;
    } else if (buf[idx] == '{' || buf[idx] == '}' || buf[idx] == '(' || buf[idx] == ')' ||
               buf[idx] == '[' || buf[idx] == ']' || buf[idx] == '.' || buf[idx] == ',' ||
               buf[idx] == ';' || buf[idx] == '+' || buf[idx] == '-' || buf[idx] == '*' ||
               buf[idx] == '/' || buf[idx] == '&' || buf[idx] == '|' || buf[idx] == '<' ||
               buf[idx] == '>' || buf[idx] == '=' || buf[idx] == '~') {
      if (0 < idx && buf[idx-1] == '/') {
        if (buf[idx] == '/') {
          prev_type = COMMENT_LINE;
        } else if (buf[idx] == '*') {
          prev_type = COMMENT_MULTI;
        }
      }
      type = (prev_type == SYMBOL) ? NONE : SYMBOL;
    } else if ('0' <= buf[idx] && buf[idx] <= '9') {
      type = (prev_type == INDENTIFIER) ? INDENTIFIER : INTEGER_CONST;
    } else if (buf[idx] == '"') {
      if (prev_type == NONE) { type = STRING; continue; }
      else if (prev_type == STRING) { prev_type = STRING_CONST; --idx; }
      type = STRING_CONST;
    } else if (buf[idx] == '\n') {
      if (prev_type == STRING) {
        printf("error: \\n in string.\n");
        exit(1);
      }
      continue;
    } else if (buf[idx] == ' ' || buf[idx] == '\t' || buf[idx] == '\r') {
      type = NONE;
    }

    if (prev_type == STRING) { type = STRING; }
    if (prev_type == COMMENT_LINE) { type = COMMENT_LINE; continue; }
    if (prev_type == COMMENT_MULTI) { type = COMMENT_MULTI; continue;}

    // printf("%s -> %s | %s\n", lex[prev_type], lex[type], buf);
    if (prev_type != NONE && prev_type != type) {
      buf[idx] = '\0';
      fseek(f, -1, SEEK_CUR); // rewind by 1
      return;
    }
    if (type != NONE) ++idx;
  }
  buf[idx] = '\0';
}

int main(int argc, char **argv) {
  char buf[MAX_SZ] = {0};

  FILE* f = t_init(argv[1]);
  while(t_has_more_tokens(f)) {
    t_advance(f, buf);
    printf("%-15s%s\n", lex[prev_type], buf);
  }

  return 0;
}