set -e
clang vm.c && ./a.out $1 && echo "vm" && clang assembler.c && ./a.out program.asm && echo "assembler" && clang -O2 hack.c && ./a.out program.hack
