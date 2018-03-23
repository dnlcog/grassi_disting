#include "aes.h"

void get_word_from_bytes(const byte *my_bytes, word *my_word) {
  *my_word = 0;
  for(int i = 0 ; i < R ; i++)
    *my_word ^= ((word)my_bytes[i] << (8 * (R - i - 1)));
}

void get_bytes_from_word(const word my_word, byte *my_bytes) {
  for(int i = 0 ; i < R ; i++)
    my_bytes[i] = (byte)(my_word >> (8 * (R - i - 1)));
}

void print_state_array(word *state_array) {

  for(int i = 0 ; i < R ; i++) {
    printf("| ");
    for(int j = 0 ; j < C ; j++) {
      printf("%2x", (state_array[j] >> (8 * (R - i - 1))) & 0xff);
      if(j == (C - 1))
	printf(" |\n");
      else
	printf(", ");
    }
  }
}
