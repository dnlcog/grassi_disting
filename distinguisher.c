#include "aes.h"

void next_plaintext(word *, const int);
int count_5_rounds_collisions(const int, const int, const AES_KEY *, big_int *);

int count_5_rounds_collisions(const int i, const int j, const AES_KEY *key, big_int *result) {

  if(key->rounds != 5) {
    fprintf(stderr, "\nError : the number of rounds in the key schedule should be 5 here. \n\n");
    return(EXIT_FAILURE);
  }

  big_int *counter_array = (big_int*)calloc(1 << (E * C), sizeof(big_int));
  word *plaintext = (word*)calloc(C, sizeof(word));
  word *ciphertext = (word*)calloc(C, sizeof(word));
  big_int x = 0;
  word c;

  for(big_int k = 0 ; k < (1 << (C * E)) ; k++) {
    aes_rounds(plaintext, ciphertext, key, 1);
    x = 0;
    for(int l = 0 ; l < C ; l++) {
      c = ciphertext[(j - l) % C] >> (8 * (R - l - 1));
      x ^= ((big_int)(c & 0xff)) << (l * E);
    }
    counter_array[x] = counter_array[x] + 1;
    next_plaintext(plaintext, i);
  }

  *result = 0;

  for(unsigned long int k = 0 ; k < (1 << (C * E)) ; k++) {
    *result += ((counter_array[k] * (counter_array[k] - 1)) >> 1);
  }

  free(counter_array);
  free(plaintext);
  free(ciphertext);

  return(EXIT_SUCCESS);
}

void next_plaintext(word *state_array, const int i) {

  int k = 0;
  word temp;
  
  while(k < R) {
    temp = ((state_array[(i + k) % C] >> (8 * (R - k - 1))) + 1) & 0xff;
#if E == 4
    temp = temp & 0x0f;
#endif
    state_array[(i + k) % C] = temp << (8 * (R - k - 1));
    if(temp == 0)
      k++;
    else
      k = R;
  }
}

int main(void) {

  AES_KEY *key = (AES_KEY*)malloc(sizeof(AES_KEY));;
  byte *useless = (byte*)malloc(sizeof(byte));

  init_AES_KEY(5, key);
  set_round_keys(useless, key);
  
  big_int *result = (big_int*)malloc(sizeof(result));
  
  count_5_rounds_collisions(0, 0, key, result);

  printf("The number of collisions for AES is\n%lu\n", *result);
  printf("Mod 8 : %d\n\n", (int)((*result) % 8));

  free_AES_KEY(key);
  free(useless);
  free(result);
  
  return(EXIT_SUCCESS);
}



