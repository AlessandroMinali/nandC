// only parses valid Jack programs(no unicode support)
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SZ 512 // functionally the limit for multi-line comment

void err(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

static char buf[MAX_SZ] = {0};
static FILE* f;
static FILE* fo;
static char nest = 0;

void t_init(char *filename) {
  f = fopen(filename, "r");
}
bool t_has_more_tokens(FILE* f) {
  return feof(f) == 0;
}
typedef enum { NONE, COMMENT_LINE, COMMENT_MULTI,
               KEYWORD, SYMBOL, INTEGER_CONST, STRING, STRING_CONST, INDENTIFIER } Lexical;
char *lex[] = { "NONE", "COMMENT_LINE", "COMMENT_MULTI",
               "keyword", "symbol", "integerConstant", "STRING", "stringConstant", "identifier" }; // TODO: remove eventually
typedef enum { _NONE, _CLASS, _METHOD, _FUNCTION, _CONSTRUCTOR, _INT, _BOOLEAN, _CHAR, _VOID,
               _VAR, _STATIC, _FIELD, _LET, _DO, _IF, _ELSE, _WHILE, _RETURN,
               _TRUE, _FALSE, _NULL, _THIS } Keyword;
char *key[] = { "none", "class", "method", "function", "constructor",
                "int", "boolean","char", "void",
                "var", "static", "field",
                "let", "do", "if", "else", "while", "return",
                "true", "false", "null", "this" }; // TODO: remove eventually
Lexical prev_type;
Keyword prev_key;
void t_advance() {
  size_t idx = 0;
  Lexical type = NONE;
  prev_key = _NONE;

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

      if (prev_type == INDENTIFIER) {
        if (strcmp(buf, "class") == 0) prev_key = _CLASS;
        else if (strcmp(buf, "method") == 0) prev_key = _METHOD;
        else if (strcmp(buf, "function") == 0) prev_key = _FUNCTION;
        else if (strcmp(buf, "constructor") == 0) prev_key = _CONSTRUCTOR;
        else if (strcmp(buf, "int") == 0) prev_key = _INT;
        else if (strcmp(buf, "boolean") == 0) prev_key = _BOOLEAN;
        else if (strcmp(buf, "char") == 0) prev_key = _CHAR;
        else if (strcmp(buf, "void") == 0) prev_key = _VOID;
        else if (strcmp(buf, "var") == 0) prev_key = _VAR;
        else if (strcmp(buf, "static") == 0) prev_key = _STATIC;
        else if (strcmp(buf, "field") == 0) prev_key = _FIELD;
        else if (strcmp(buf, "let") == 0) prev_key = _LET;
        else if (strcmp(buf, "do") == 0) prev_key = _DO;
        else if (strcmp(buf, "if") == 0) prev_key = _IF;
        else if (strcmp(buf, "else") == 0) prev_key = _ELSE;
        else if (strcmp(buf, "while") == 0) prev_key = _WHILE;
        else if (strcmp(buf, "return") == 0) prev_key = _RETURN;
        else if (strcmp(buf, "true") == 0) prev_key = _TRUE;
        else if (strcmp(buf, "false") == 0) prev_key = _FALSE;
        else if (strcmp(buf, "null") == 0) prev_key = _NULL;
        else if (strcmp(buf, "this") == 0) prev_key = _THIS;

        if (prev_key != _NONE) prev_type = KEYWORD;
      }
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

void ce_init(char *filename) {
  char out[MAX_SZ];
  for(uint16_t i = 0; i < MAX_SZ; ++i) {
    if (filename[i] == '.' || filename[i] == '\0') {
      out[i] = '\0';
      break;
    }
    out[i] = filename[i];
  }
  strcat(out, "1.xml"); // TODO: remove 1 after testing
  fo = fopen(out, "w");
}
void ce_compile_term() { // Todo: array and sub rt handling
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fprintf(fo, "<%s> %s </%s>\n", lex[prev_type], buf, lex[prev_type]);
}
void ce_compile_class_var_dec() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<classVarDec>\n", 14, 1,fo);
  ++nest;

  ce_compile_term();

  t_advance();
  if (!(prev_key == _INT ||
        prev_key == _CHAR ||
        prev_key == _BOOLEAN ||
        prev_type == INDENTIFIER)) err("syntax: expected type");
  else ce_compile_term();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_term();

  t_advance();
  while(buf[0] == ',') {
    ce_compile_term();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_term();

    t_advance();
  }

  if (buf[0] != ';') err("syntax: expected ;");
  else ce_compile_term();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</classVarDec>\n", 15, 1, fo);
}
void ce_compile_parameter_list() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<parameterList>\n", 16, 1,fo);
  ++nest;

  if (buf[0] != ')') {
    if (!(prev_key == _INT ||
          prev_key == _CHAR ||
          prev_key == _BOOLEAN ||
          prev_type == INDENTIFIER)) err("syntax: expected type");
    else ce_compile_term();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_term();

    t_advance();
    while(buf[0] == ',') {
      ce_compile_term();

      t_advance();
      if (!(prev_key == _INT ||
          prev_key == _CHAR ||
          prev_key == _BOOLEAN ||
          prev_type == INDENTIFIER)) err("syntax: expected type");
      else ce_compile_term();

      t_advance();
      if (prev_type != INDENTIFIER) err("syntax: expected identifier");
      else ce_compile_term();

      t_advance();
    }
  }

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</parameterList>\n", 17, 1, fo);
}
void ce_compile_var_dec() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<varDec>\n", 9, 1,fo);
  ++nest;

  ce_compile_term();

  t_advance();
  if (!(prev_key == _INT ||
        prev_key == _CHAR ||
        prev_key == _BOOLEAN ||
        prev_type == INDENTIFIER)) err("syntax: expected type");
  else ce_compile_term();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_term();

  t_advance();
  while(buf[0] == ',') {
    ce_compile_term();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_term();

    t_advance();
  }

  if (!(prev_type == SYMBOL || buf[0] == ';')) err("syntax: expected ;");
  else ce_compile_term();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</varDec>\n", 10, 1, fo);
}
void ce_compile_statements() {

}

void ce_compile_subroutine() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<subroutineDec>\n", 16, 1,fo);
  ++nest;

  ce_compile_term();

  t_advance();
  if (!(prev_key == _VOID ||
        prev_key == _INT ||
        prev_key == _CHAR ||
        prev_key == _BOOLEAN ||
        prev_type == INDENTIFIER)) err("syntax: expected void | type");
  else ce_compile_term();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_term();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected (");
  else ce_compile_term();

  t_advance();
  ce_compile_parameter_list();

  if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
  else ce_compile_term();

  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<subroutineBody>\n", 17, 1,fo);
  ++nest;

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected {");
  else ce_compile_term();

  t_advance();
  while(prev_key == _VAR) {
    ce_compile_var_dec();
    t_advance();
  }

  // TODO: statements
  // ce_compile_statements();

  if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected }");
  else ce_compile_term();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</subroutineBody>\n", 18, 1, fo);

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</subroutineDec>\n", 17, 1, fo);
}
void ce_compile_class() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<class>\n", 8, 1,fo);
  ++nest;

  t_advance();
  if (prev_key != _CLASS) err("syntax: expected class");
  else ce_compile_term();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected class");
  else ce_compile_term();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '{')) err("syntax: expected {");
  else ce_compile_term();

  t_advance();
  while(prev_key == _STATIC || prev_key == _FIELD) {
    ce_compile_class_var_dec();
    t_advance();
  }

  while(prev_key == _CONSTRUCTOR || prev_key == _FUNCTION || prev_key == _METHOD) {
    ce_compile_subroutine();
    t_advance();
  }

  if (!(prev_type == SYMBOL || buf[0] == '}')) err("syntax: expected }");
  else ce_compile_term();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</class>\n", 9, 1, fo);
}
// void ce_compile_do();
// void ce_compile_let();
// void ce_compile_while();
// void ce_compile_return();
// void ce_compile_if();
// void ce_compile_expression();
// void ce_compile_expression_list();

int main(int argc, char **argv) {
  char *filename = argv[1];

  t_init(filename);
  ce_init(filename);
  // while(t_has_more_tokens(f)) {
  //   t_advance(f, buf);
  //   printf("%-15s%s\n", lex[prev_type], buf);
  // }
  ce_compile_class();

  return 0;
}