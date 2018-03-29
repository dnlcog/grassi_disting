/* key_schedule.c */


/*
This file contains all the relevant functions
for the key generation or expansion.
*/




#include "aes.h"



/* Round constants for the rijndael key schedule. */
static const word32 rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000
};




int key_schedule_inner_function(word32 *my_word, int index);

/* Rijndael key schedule inner function. */
int key_schedule_inner_function(word32 *my_word, int index) {

  static word32 temp = 0;
  static word32 mask = (1 << (8 * (R - 1))) - 1;

  temp = (*my_word >> (8 * (R - 1))) & 0xff;
  temp ^= ((*my_word & mask) << 8);

  *my_word = 0;
  for(int i = 0 ; i < R ; i++)
    *my_word ^= (word32)(sbox[(temp >> (8 * (R - i - 1))) & 0xff]) << (8 * (R - i - 1));

  *my_word ^= rcon[index - 1];
  return(EXIT_SUCCESS);
}


/* Rijndael key expansion procedure. */
int set_round_keys_rijndael(const byte8 *userKey, AES_KEY *key) {

  word32 *rk;
  int nb_rounds;
  int i;
  word32 temp;

  nb_rounds = key->rounds;
  rk = key->rd_key;

  for(i = 0 ; i < K ; i++)
    get_word_from_bytes(userKey + (R * i), rk + i);

  for(i = K ; i < (nb_rounds + 1) * C ; i++) {
    temp = rk[i-1];
   
    if((i % K) == 0)
      key_schedule_inner_function(&temp, i/K);

    rk[i] = rk[i - K] ^ temp;
  }

  return(EXIT_SUCCESS);
}



/* Key schedule set with random keys. */
int set_round_keys_random(AES_KEY *key) {

  word32 *rk = key->rd_key;
  int nb_rounds = key->rounds;

  for(int i = 0 ; i < (nb_rounds + 1) * C ; i++) {
    rk[i] = (word32)(i);
#if E == 4
    rk[i] &= 0x0f0f0f0f;
#endif
  }

  return(EXIT_SUCCESS);
}



void init_AES_KEY(const int nb_rounds, AES_KEY *key) {
  key->rounds = nb_rounds;
  key->rd_key = (word32*)malloc((nb_rounds + 1) * C * sizeof(word32));
}



void free_AES_KEY(AES_KEY *key) {
  free(key->rd_key);
  free(key);
}












