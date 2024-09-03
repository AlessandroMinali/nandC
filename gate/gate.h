#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// print helper
void printbits(uint16_t v) {
  for(char i = 15; i >= 0; i--) putchar('0' + ((v >> i) & 1));
    putchar('\n');
}

// bit manipulation
bool get_bit(uint16_t a, uint8_t pos) {
  return (a >> pos) & 0x1;
}
uint16_t set_bit(uint16_t a, bool v, uint8_t pos) {
  return a | (v << pos);
}
uint16_t toggle_bit(uint16_t a, uint8_t pos) {
  return a ^ (0x1 << pos);
}
uint16_t clear_bit(uint16_t a, uint8_t pos) {
  return a & ~(0x1 << pos);
}

uint16_t gate_bit(uint16_t a, uint16_t b, uint8_t pos, bool (*gate)(bool, bool)) {
  return set_bit(clear_bit(a, pos), gate(get_bit(a, pos), get_bit(b, pos)), pos);
}