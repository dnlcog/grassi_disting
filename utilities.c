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

void print_bytes(byte *my_bytes, unsigned int n) {
  for(unsigned int i = 0 ; i < n ; i++)
    printf("%2x ", my_bytes[i]);
  printf("\n");
}

void get_state_array_from_bytes(const byte *my_bytes, word *state_array) {
  for(int j = 0 ; j < C ; j++)
    get_word_from_bytes(my_bytes + (R * j), state_array + j);
}

void get_bytes_from_state_array(const word *state_array, byte *my_bytes) {
  for(int j = 0 ; j < C ; j++)
    get_bytes_from_word(state_array[j], my_bytes + (R * j));
}

