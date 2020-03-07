#include <stdlib.h>

int bit_masks[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
char* to_bits(char *data) {
  char *text = malloc(sizeof(char) * 8);
  char c;

  for (int i = 0; i < 8; i++) {
    c = (*data & bit_masks[i]) >> (7 - i);
    *(text + i) = c == 0 ? '0' : '1';
  }

  return text;
}