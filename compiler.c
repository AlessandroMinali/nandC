// only parses valid Jack programs(no unicode support)
// rote implementation from "The Elements of Computing Systems" text
// almost no error checking or considerations to optimization
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#define MAX_SZ 512 // functionally the limit for multi-line comment

void err(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

static char buf[MAX_SZ] = {0};
static FILE* f;
static FILE* fo;
static char nest = 0;

// forward declarations
void ce_compile_statements();
void ce_compile_expression();
void ce_compile_expression_list();

void t_init(char *filename) {
  f = fopen(filename, "r");
}
bool t_has_more_tokens(FILE* f) {
  return feof(f) == 0;
}
typedef enum { NONE, COMMENT_LINE, COMMENT_MULTI,
               KEYWORD, SYMBOL, INTEGER_CONST, STRING, STRING_CONST, INDENTIFIER } Lexical;
char *lex[] = { "NONE", "COMMENT_LINE", "COMMENT_MULTI",
               "keyword", "symbol", "integerConstant", "STRING", "stringConstant", "identifier" };
typedef enum { _NONE, _CLASS, _METHOD, _FUNCTION, _CONSTRUCTOR, _INT, _BOOLEAN, _CHAR, _VOID,
               _VAR, _STATIC, _FIELD, _LET, _DO, _IF, _ELSE, _WHILE, _RETURN,
               _TRUE, _FALSE, _NULL, _THIS } Keyword;
char *key[] = { "none", "class", "method", "function", "constructor",
                "int", "boolean","char", "void",
                "var", "static", "field",
                "let", "do", "if", "else", "while", "return",
                "true", "false", "null", "this" };
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
  strcat(out, ".xml");
  fo = fopen(out, "w");
}
void ce_compile_terminal() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fprintf(fo, "<%s> %s </%s>\n", lex[prev_type], buf, lex[prev_type]);
}
void ce_compile_class_var_dec() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<classVarDec>\n", 14, 1,fo);
  ++nest;

  ce_compile_terminal();

  t_advance();
  if (!(prev_key == _INT ||
        prev_key == _CHAR ||
        prev_key == _BOOLEAN ||
        prev_type == INDENTIFIER)) err("syntax: expected type");
  else ce_compile_terminal();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_terminal();

  t_advance();
  while(buf[0] == ',') {
    ce_compile_terminal();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_terminal();

    t_advance();
  }

  if (buf[0] != ';') err("syntax: expected ;");
  else ce_compile_terminal();

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
    else ce_compile_terminal();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_terminal();

    t_advance();
    while(buf[0] == ',') {
      ce_compile_terminal();

      t_advance();
      if (!(prev_key == _INT ||
          prev_key == _CHAR ||
          prev_key == _BOOLEAN ||
          prev_type == INDENTIFIER)) err("syntax: expected type");
      else ce_compile_terminal();

      t_advance();
      if (prev_type != INDENTIFIER) err("syntax: expected identifier");
      else ce_compile_terminal();

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

  ce_compile_terminal();

  t_advance();
  if (!(prev_key == _INT ||
        prev_key == _CHAR ||
        prev_key == _BOOLEAN ||
        prev_type == INDENTIFIER)) err("syntax: expected type");
  else ce_compile_terminal();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_terminal();

  t_advance();
  while(buf[0] == ',') {
    ce_compile_terminal();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_terminal();

    t_advance();
  }

  if (!(prev_type == SYMBOL || buf[0] == ';')) err("syntax: expected ;");
  else ce_compile_terminal();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</varDec>\n", 10, 1, fo);
}
void ce_compile_term() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<term>\n", 7, 1,fo);
  ++nest;

  if (prev_type == INTEGER_CONST || prev_type == STRING_CONST ||
      prev_type == KEYWORD) {
    ce_compile_terminal();

    t_advance();
  } else if (prev_type == INDENTIFIER) {
    ce_compile_terminal();

    t_advance();
    if (prev_type == SYMBOL) {
      if (buf[0] == '[') {
        ce_compile_terminal();

        t_advance();
        ce_compile_expression();

        if (buf[0] != ']') err("syntax: expected ]");
        else ce_compile_terminal();

        t_advance();
      } else if (buf[0] == '(') {
        ce_compile_terminal();

        t_advance();
        ce_compile_expression_list();

        if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
        else ce_compile_terminal();

        t_advance();
      } else if (buf[0] == '.') {
        ce_compile_terminal();

        t_advance();
        if (prev_type != INDENTIFIER) err("syntax: expected identifier");
        else ce_compile_terminal();

        t_advance();
        if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected (");
        else ce_compile_terminal();

        t_advance();
        ce_compile_expression_list();

        if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
        else ce_compile_terminal();

        t_advance();
      } else {
        // pass-through
      }
    }
  } else if (prev_type == SYMBOL) {
    if (buf[0] == '(') {
      ce_compile_terminal();

      t_advance();
      ce_compile_expression();

      if (buf[0] != ')') err("syntax: expected )");
      else ce_compile_terminal();

      t_advance();
    } else if (buf[0] == '-' || buf[0] == '~') {
      ce_compile_terminal();

      t_advance();
      ce_compile_term();
    } else { err("syntax: unexpected symbol"); }
  } else { err("syntax: unexpected term"); }

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</term>\n", 8, 1, fo);
}
void ce_compile_expression() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<expression>\n", 13, 1,fo);
  ++nest;

  ce_compile_term();

  while (prev_type == SYMBOL && (
      buf[0] == '+' || buf[0] == '-' || buf[0] == '*' || buf[0] == '/' ||
      buf[0] == '&' || buf[0] == '|' || buf[0] == '<' || buf[0] == '>' || buf[0] == '=')) {
    ce_compile_terminal();

    t_advance();
    ce_compile_term();
  }

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</expression>\n", 14, 1, fo);
}
void ce_compile_expression_list() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<expressionList>\n", 17, 1,fo);
  ++nest;

  if (buf[0] != ')') {
    ce_compile_expression();

    while(buf[0] == ',') {
      ce_compile_terminal();

      t_advance();
      ce_compile_expression();
    }
  }

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</expressionList>\n", 18, 1, fo);
}
void ce_compile_let() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<letStatement>\n", 15, 1,fo);
  ++nest;

  ce_compile_terminal();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_terminal();

  t_advance();
  if (prev_type == SYMBOL && buf[0] == '[') {
    ce_compile_terminal();

    t_advance();
    ce_compile_expression();

    if (!(prev_type == SYMBOL || buf[0] == ']')) err("syntax: expected ]");
    else ce_compile_terminal();

    t_advance();
  }

  if (!(prev_type == SYMBOL || buf[0] == '=')) err("syntax: expected =");
  else ce_compile_terminal();

  t_advance();
  ce_compile_expression();

  if (!(prev_type == SYMBOL || buf[0] == ';')) err("syntax: expected ;");
  else ce_compile_terminal();

  t_advance();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</letStatement>\n", 16, 1, fo);
}
void ce_compile_if() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<ifStatement>\n", 14, 1,fo);
  ++nest;

  ce_compile_terminal();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected (");
  else ce_compile_terminal();

  t_advance();
  ce_compile_expression();

  if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
  else ce_compile_terminal();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '{')) err("syntax: expected {");
  else ce_compile_terminal();

  t_advance();
  ce_compile_statements();

  if (!(prev_type == SYMBOL || buf[0] == '}')) err("syntax: expected }");
  else ce_compile_terminal();

  t_advance();
  if (prev_key == _ELSE) {
    ce_compile_terminal();

    t_advance();
    if (!(prev_type == SYMBOL || buf[0] == '{')) err("syntax: expected {");
    else ce_compile_terminal();

    t_advance();
    ce_compile_statements();

    if (!(prev_type == SYMBOL || buf[0] == '}')) err("syntax: expected }");
    else ce_compile_terminal();

    t_advance();
  }

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</ifStatement>\n", 15, 1, fo);
}
void ce_compile_while() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<whileStatement>\n", 17, 1,fo);
  ++nest;

  ce_compile_terminal();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected (");
  else ce_compile_terminal();

  t_advance();
  ce_compile_expression();

  if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
  else ce_compile_terminal();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '{')) err("syntax: expected {");
  else ce_compile_terminal();

  t_advance();
  ce_compile_statements();

  if (!(prev_type == SYMBOL || buf[0] == '}')) err("syntax: expected }");
  else ce_compile_terminal();

  t_advance();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</whileStatement>\n", 18, 1, fo);
}

void ce_compile_do() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<doStatement>\n", 14, 1,fo);
  ++nest;

  ce_compile_terminal();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_terminal();

  t_advance();
  if (prev_type != SYMBOL || (buf[0] != '(' && buf[0] != '.')) err("syntax: expected subroutine call");
  if (buf[0] == '(') {
    ce_compile_terminal();

    t_advance();
    ce_compile_expression_list();

    if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
    else ce_compile_terminal();
  } else if (buf[0] == '.') {
    ce_compile_terminal();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_terminal();

    t_advance();
    if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected (");
    else ce_compile_terminal();

    t_advance();
    ce_compile_expression_list();

    if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
    else ce_compile_terminal();
  } else {
    err("syntax: invalid subroutine call");
  }

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == ';')) err("syntax: expected ;");
  else ce_compile_terminal();

  t_advance();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</doStatement>\n", 15, 1, fo);
}
void ce_compile_return() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<returnStatement>\n", 18, 1,fo);
  ++nest;

  ce_compile_terminal();

  t_advance();
  if(buf[0] != ';') ce_compile_expression();

  if (!(prev_type == SYMBOL || buf[0] == ';')) err("syntax: expected ;");
  else ce_compile_terminal();

  t_advance();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</returnStatement>\n", 19, 1, fo);
}
void ce_compile_statements() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<statements>\n", 13, 1,fo);
  ++nest;

  while(1) {
    if (prev_key == _LET) ce_compile_let();
    else if (prev_key == _IF) ce_compile_if();
    else if (prev_key == _WHILE) ce_compile_while();
    else if (prev_key == _DO) ce_compile_do();
    else if (prev_key == _RETURN) ce_compile_return();
    else break;
  }

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</statements>\n", 14, 1,fo);
}
void ce_compile_class() {
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("<class>\n", 8, 1,fo);
  ++nest;

  t_advance();
  if (prev_key != _CLASS) err("syntax: expected class");
  else ce_compile_terminal();

  t_advance();
  if (prev_type != INDENTIFIER) err("syntax: expected identifier");
  else ce_compile_terminal();

  t_advance();
  if (!(prev_type == SYMBOL || buf[0] == '{')) err("syntax: expected {");
  else ce_compile_terminal();

  t_advance();
  while(prev_key == _STATIC || prev_key == _FIELD) {
    ce_compile_class_var_dec();
    t_advance();
  }

  while(prev_key == _CONSTRUCTOR || prev_key == _FUNCTION || prev_key == _METHOD) {
    for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
    fwrite("<subroutineDec>\n", 16, 1,fo);
    ++nest;

    ce_compile_terminal();

    t_advance();
    if (!(prev_key == _VOID ||
          prev_key == _INT ||
          prev_key == _CHAR ||
          prev_key == _BOOLEAN ||
          prev_type == INDENTIFIER)) err("syntax: expected void | type");
    else ce_compile_terminal();

    t_advance();
    if (prev_type != INDENTIFIER) err("syntax: expected identifier");
    else ce_compile_terminal();

    t_advance();
    if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected (");
    else ce_compile_terminal();

    t_advance();
    ce_compile_parameter_list();

    if (!(prev_type == SYMBOL || buf[0] == ')')) err("syntax: expected )");
    else ce_compile_terminal();

    for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
    fwrite("<subroutineBody>\n", 17, 1,fo);
    ++nest;

    t_advance();
    if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected {");
    else ce_compile_terminal();

    t_advance();
    while(prev_key == _VAR) {
      ce_compile_var_dec();
      t_advance();
    }

    ce_compile_statements();

    if (!(prev_type == SYMBOL || buf[0] == '(')) err("syntax: expected }");
    else ce_compile_terminal();

    t_advance();

    --nest;
    for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
    fwrite("</subroutineBody>\n", 18, 1, fo);

    --nest;
    for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
    fwrite("</subroutineDec>\n", 17, 1, fo);
  }

  if (!(prev_type == SYMBOL || buf[0] == '}')) err("syntax: expected }");
  else ce_compile_terminal();

  --nest;
  for(char i = 0; i < nest; ++i) fwrite("  ", 1, 2,fo);
  fwrite("</class>\n", 9, 1, fo);
}

int main(int argc, char **argv) {
  char path[MAX_SZ];
  char fname[MAX_SZ];
  size_t len = strlen(argv[1]);
  memcpy(path, argv[1], len+1);
  if (path[len-1] != '/') {
    path[len] = '/';
    ++len;
  }

  bool once = true;
  DIR *d = opendir(argv[1]);
  while(1) {
    if (d != NULL) {
      struct dirent *item;
      while((item = readdir(d))) {
        size_t sz = strlen(item->d_name);
        if (item->d_type == DT_REG &&
            item->d_name[sz-5] == '.' &&
            item->d_name[sz-4] == 'j' &&
            item->d_name[sz-3] == 'a' &&
            item->d_name[sz-2] == 'c' &&
            item->d_name[sz-1] == 'k') {
            memcpy(fname, item->d_name, sz+1);
            memcpy(&path[len], item->d_name, sz+1);
          break;
        }
      }
      if (!item) { break; } // traversed entire dir
    } else if (once) { once = false; }
    else { break; }

    t_init(path);
    ce_init(fname);
    ce_compile_class();
  }

  return 0;
}