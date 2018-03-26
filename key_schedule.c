#include "aes.h"

#if ROUNDKEYS == rijndael

static const word rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000
};

#if (R == 4) && (C == 4)

int set_round_keys(const byte *userKey, AES_KEY *key) {

  word *rk;
  int nb_rounds;
  int i = 0;
  word temp;

  nb_rounds = key->rounds;
  rk = key->rd_key;
  
  get_word_from_bytes(userKey     , rk    );
  get_word_from_bytes(userKey +  4, rk + 1);
  get_word_from_bytes(userKey +  8, rk + 2);
  get_word_from_bytes(userKey + 12, rk + 3);
  while (1) {
    temp  = rk[3];
    rk[4] = rk[0] ^
      (Te2[(temp >> 16) & 0xff] & 0xff000000) ^
      (Te3[(temp >>  8) & 0xff] & 0x00ff0000) ^
      (Te0[(temp      ) & 0xff] & 0x0000ff00) ^
      (Te1[(temp >> 24)       ] & 0x000000ff) ^
      rcon[i];
    rk[5] = rk[1] ^ rk[4];
    rk[6] = rk[2] ^ rk[5];
    rk[7] = rk[3] ^ rk[6];
    if (++i == nb_rounds) {
      return(EXIT_SUCCESS);
    }
    rk += 4;
  }
}

#else
int key_schedule_inner_function(word *, int);

int set_round_keys(const byte *userKey, AES_KEY *key) {

  word *rk;
  int nb_rounds;
  
  nb_rounds = key->rounds;
  rk = key->rd_key;

  for(int i = 0 ; i < C ; i++)
    get_word_from_bytes(userKey + (R * i), rk + i);

  word temp;

  for(int i = C ; i < (nb_rounds + 1) * C ; i++) {
    temp = rk[i-1];
   
    if(!(i % C))
      key_schedule_inner_function(&temp);

    rk[i] = rk[i - C] ^ temp;
  }

  return(EXIT_SUCCESS);
}

int key_schedule_inner_function(word *my_word, int index) {
  byte my_bytes[R];
  get_bytes_from_word(*my_word, my_bytes);
  for(int i = 0 ; i < R ; i++)
    my_bytes[i] = sbox[my_bytes[(i + 1) % R]];
  get_word_from_bytes(my_bytes, my_word);
  *my_word ^= rcon[index/C];
  return(EXIT_SUCCESS);
}

#endif

#elif ROUNDKEYS == random

int set_round_keys(const byte *useless, AES_KEY *key) {

  printf("ici\n");
  word *rk = key->rd_key;
  int nb_rounds = key->rounds;

  for(int i = 0 ; i < (nb_rounds + 1) * C ; i++) {
    rk[i] = (word)(i);
#if E == 4
    rk[i] &= 0x0f0f0f0f;
#endif
  }

  return(EXIT_SUCCESS);
}





#endif
	


void init_AES_KEY(const int nb_rounds, AES_KEY *key) {
  key->rounds = nb_rounds;
  key->rd_key = (word*)malloc((nb_rounds + 1) * C * sizeof(word));
}



void free_AES_KEY(AES_KEY *key) {
  free(key->rd_key);
  free(key);
}












