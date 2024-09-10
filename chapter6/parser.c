#include <stdio.h>
#include <stdbool.h>
#include <string.h>

FILE* p_init(char const *filename) {
  return fopen(filename, "r");
}
bool p_has_more_commands(FILE* f) { // PERF: fgets already can detect this
  return feof(f) == 0;
}
void p_advance(FILE* f, char *buf) { // PERF: better to read all at once
  fgets(buf, 512, f);
  while(1) {
    char *s = buf;
    while(*s != 0 && *s != '\n') {
      if (*(s++) != ' ') { return; } // NOTE: we skip empty lines
    }
    fgets(buf, 512, f);
  }
}
typedef enum Command { A_COMMAND, C_COMMAND, L_COMMAND, };
Command commandType(char *buf) {
  // TODO
}

int main(int argc, char const *argv[])
{
  char command_buf[512] = {0};
  FILE* f = p_init(argv[1]);
  if (p_has_more_commands(f)) {
    p_advance(f, command_buf);
  }
  printf("%s", command_buf);

  return 0;
}
