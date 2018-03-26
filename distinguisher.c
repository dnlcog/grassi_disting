#include "aes.h"

void next_plaintext(word *, const int);
int count_5_rounds_collisions(const int, const int, const AES_KEY *, big_int *);

int count_5_rounds_collisions(const int i, const int j, const AES_KEY *key, big_int *result) {

  if(key->rounds != 5) {
    fprintf(stderr, "\nError : the number of rounds in the key schedule should be 5 here. \n\n");
    return(EXIT_FAILURE);
  }
  big_int bound = (big_int)(1) << (C * E);

  big_int *counter_array = (big_int*)calloc(bound, sizeof(big_int));

  word *plaintext = (word*)calloc(C, sizeof(word));
  word *ciphertext = (word*)calloc(C, sizeof(word));

  big_int x = 0;

  word c;

  for(big_int k = 0 ; k < bound ; k++) {
    aes_rounds(plaintext, ciphertext, key, 1, (k == bound - 1));
    x = 0;
    for(int l = 0 ; l < C ; l++) {
      c = (ciphertext[(C + j - l) % C] >> (8 * (R - l - 1))) & 0xff;
      x ^= ((big_int)(c)) << (l * E);
    }
    counter_array[x] = counter_array[x] + 1;
    next_plaintext(plaintext, i);
  }

  *result = 0;

  for(unsigned long int k = 0 ; k < bound ; k++) {
    *result += ((counter_array[k] * (counter_array[k] - 1)) >> 1);
  }

  big_int max = counter_array[0];
  big_int min = counter_array[0];
  for(unsigned long int k = 0 ; k < bound ; k++) {
    if(counter_array[k] < min)
       min = counter_array[k];
    if(counter_array[k] > max)
       max = counter_array[k];
  }

  printf("%lu : %lu\n", min, max);
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

  AES_KEY *key = (AES_KEY*)malloc(sizeof(AES_KEY));
  byte *useless = (byte*)malloc(sizeof(byte));

  init_AES_KEY(5, key);
  set_round_keys(useless, key);
  
  big_int **result = (big_int**)malloc(C * sizeof(big_int*));

  for(int i = 0 ; i < C ; i++) {
    result[i] = (big_int*)malloc(C * sizeof(big_int));
    for(int j = 0 ; j < C ; j++)
      count_5_rounds_collisions(i, j, key, result[i] + j);
  }

  printf("\nFull\n");

  printf("i\\j    ");

  for(int i = 0 ; i < C ; i++)
    printf("%-10d ", i+1);
  printf("\n");
  
  for(int i = 0 ; i < C ; i++) {
    printf("%d ", i+1);
    for(int j = 0 ; j < C ; j++)
      printf("%10lu ", result[i][j]);
    printf("\n");
  }
  
  printf("\nDiv by  8\n");

  printf("i\\j    ");

  for(int i = 0 ; i < C ; i++)
    printf("%-10d ", i+1);
  printf("\n");
  
  for(int i = 0 ; i < C ; i++) {
    printf("%d ", i+1);
    for(int j = 0 ; j < C ; j++)
      printf("%10lu ", result[i][j] / 8);
    printf("\n");
  }
  
  for(int i = 0 ; i < C ; i++)
    free(result[i]);
  free(result);
  free_AES_KEY(key);
  free(useless);
  
  return(EXIT_SUCCESS);
}



