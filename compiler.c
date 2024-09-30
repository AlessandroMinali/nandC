// only parses valid Jack programs(no unicode support)
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SZ 512 // functionally the limit for multi-line comment 

FILE* t_init(char *filename) {
  return fopen(filename, "r");
}
bool t_has_more_tokens(FILE* f) {
  return feof(f) == 0;
}
typedef enum { NONE, COMMENT_LINE, COMMENT_MULTI,
               KEYWORD, SYMBOL, INTEGER_CONST, STRING, STRING_CONST, INDENTIFIER,
               _CLASS, _METHOD, _FUNCTION, _CONSTRUCTOR, _INT, _BOOLEAN, _CHAR, _VOID,
               _VAR, _STATIC, _FIELD, _LET, _DO, _IF, _ELSE, _WHILE, _RETURN,
               _TRUE, _FALSE, _NULL, _THIS } Lexical;
char *lex[] = { "NONE", "COMMENT_LINE", "COMMENT_MULTI",
               "KEYWORD", "SYMBOL", "INTEGER_CONST", "STRING", "STRING_CONST", "INDENTIFIER",
               "CLASS", "METHOD", "FUNCTION", "CONSTRUCTOR", "INT", "BOOLEAN", "CHAR", "VOID",
               "VAR", "STATIC", "FIELD", "LET", "DO", "IF", "ELSE", "WHILE", "RETURN",
               "TRUE", "FALSE", "NULL", "THIS" }; // TODO: remove eventually
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
      type = (prev_type == SYMBOL) ? NONE : SYMBOL; // symbol is one char
    } else if ('0' <= buf[idx] && buf[idx] <= '9') {
      type = (prev_type == INDENTIFIER) ? INDENTIFIER : INTEGER_CONST; // ident may contain int
    } else if (buf[idx] == '"') {
      if (prev_type == NONE) {
        type = STRING;
        continue; // ignore first "
      } else if (prev_type == STRING) {
        prev_type = STRING_CONST;
        --idx; // ignore last "
      }
      type = STRING_CONST;
    } else if (buf[idx] == '\n') {
      if (prev_type == STRING) {
        printf("error: \\n in string.\n");
        exit(1);
      }
      continue;
    } else if (buf[idx] == ' ' || buf[idx] == '\t' || buf[idx] == '\r') {
      type = NONE; // whitespace lexical boundary
    }

    if (prev_type == STRING) { type = STRING; }
    else if (prev_type == COMMENT_LINE) { type = COMMENT_LINE; continue; }
    else if (prev_type == COMMENT_MULTI) { type = COMMENT_MULTI; continue; }

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
/* the following outputs are found in the global <prev_type>
Lexical t_token_type();
Lexical t_key_word();
Lexical t_symbol();
Lexical t_identifier();
Lexical t_int_val();
Lexical t_string_val();
*/

int main(int argc, char **argv) {
  char buf[MAX_SZ] = {0};

  FILE* f = t_init(argv[1]);
  while(t_has_more_tokens(f)) {
    t_advance(f, buf);
    if (prev_type == INDENTIFIER) {
      if (strcmp(buf, "class") == 0) prev_type = _CLASS;
      if (strcmp(buf, "method") == 0) prev_type = _METHOD;
      if (strcmp(buf, "function") == 0) prev_type = _CLASS;
      if (strcmp(buf, "constructor") == 0) prev_type = _CONSTRUCTOR;
      if (strcmp(buf, "int") == 0) prev_type = _INT;
      if (strcmp(buf, "boolean") == 0) prev_type = _BOOLEAN;
      if (strcmp(buf, "char") == 0) prev_type = _CHAR;
      if (strcmp(buf, "void") == 0) prev_type = _VOID;
      if (strcmp(buf, "var") == 0) prev_type = _VAR;
      if (strcmp(buf, "static") == 0) prev_type = _STATIC;
      if (strcmp(buf, "field") == 0) prev_type = _FIELD;
      if (strcmp(buf, "let") == 0) prev_type = _LET;
      if (strcmp(buf, "do") == 0) prev_type = _DO;
      if (strcmp(buf, "if") == 0) prev_type = _IF;
      if (strcmp(buf, "else") == 0) prev_type = _ELSE;
      if (strcmp(buf, "while") == 0) prev_type = _WHILE;
      if (strcmp(buf, "return") == 0) prev_type = _RETURN;
      if (strcmp(buf, "true") == 0) prev_type = _TRUE;
      if (strcmp(buf, "false") == 0) prev_type = _FALSE;
      if (strcmp(buf, "null") == 0) prev_type = _NULL;
      if (strcmp(buf, "this") == 0) prev_type = _THIS;
    }
    printf("%-15s%s\n", lex[prev_type], buf);
  }

  return 0;
}