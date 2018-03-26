#include "aes.h"



#if (C == 4) && (R == 4)

void aes_rounds(const word *in, word *out, const AES_KEY *key, const int last_round_without_mixcolumns, int clear) {

  static word *s;
  static word *t;
  static int r;
  static int i = 0;
  static const word *rk;

  t = (word*)malloc(C * sizeof(word));
  s =(word*)malloc(C * sizeof(word));
  r = key->rounds;
  rk = key->rd_key;






#ifdef DEBUG_AES_CORE
  byte *my_bytes = (byte*)malloc(C * R * sizeof(byte));
  get_bytes_from_state_array(rk, my_bytes);
  printf("\nThe key is :\n");
  print_bytes(my_bytes, C * R);
  
  get_bytes_from_state_array(in, my_bytes);
  printf("\nThe input is :\n");
  print_bytes(my_bytes, C * R);
#endif






  for(i = 0 ; i < C ; i++)
    s[i] = in[i] ^ rk[i];

  if(last_round_without_mixcolumns)
    r--;

  while(1) {







#ifdef DEBUG_AES_CORE
    get_bytes_from_state_array(rk, my_bytes);
    printf("\nThe key is :\n");
    print_bytes(my_bytes, C * R);
  
    get_bytes_from_state_array(s, my_bytes);
    printf("\nAfter round %d :\n", key->rounds - r - 1);
    print_bytes(my_bytes, C * R);
#endif






    if(r == 0) {
      for(i = 0 ; i < C ; i++)
	out[i] = s[i];
      rk += C;
      break;
    }

    for(i = 0 ; i < C ; i++) {
      t[i] =
	Te0[(s[(i + 0) % C] >> 24) & 0xff] ^
	Te1[(s[(i + 1) % C] >> 16) & 0xff] ^
	Te2[(s[(i + 2) % C] >>  8) & 0xff] ^
	Te3[(s[(i + 3) % C]      ) & 0xff] ^
	rk[i + C];
    }

    r--;






#ifdef DEBUG_AES_CORE
    get_bytes_from_state_array(rk + 4, my_bytes);
    printf("\nThe key is :\n");
    print_bytes(my_bytes, C * R);
  
    get_bytes_from_state_array(t, my_bytes);
    printf("\nAfter round %d :\n", key->rounds - r - 1);
    print_bytes(my_bytes, C * R);
#endif



    rk += 8;



    if(r == 0) {
      for(i = 0 ; i < C ; i++)
	out[i] = t[i];  
      break;
    }
  
    for(i = 0 ; i < C ; i++) {
      s[i] =
	Te0[(t[(i + 0) % C] >> 24) & 0xff] ^
	Te1[(t[(i + 1) % C] >> 16) & 0xff] ^
	Te2[(t[(i + 2) % C] >>  8) & 0xff] ^
	Te3[(t[(i + 3) % C]      ) & 0xff] ^
	rk[i];
    }

    r--;

  }

  if(last_round_without_mixcolumns) {
    for(i = 0 ; i < C ; i++) {
      s[i] =
	(sbox[(out[(i + 0) % C] >> 24)       ] << 24) ^
	(sbox[(out[(i + 1) % C] >> 16) & 0xff] << 16) ^
	(sbox[(out[(i + 2) % C] >>  8) & 0xff] <<  8) ^
	(sbox[(out[(i + 3) % C]      ) & 0xff]      ) ^
	rk[i];
    }

    for(i = 0 ; i < C ; i++)
      out[i] = s[i];
  }

  if(clear) {
    free(t);
    free(s);
  }

#ifdef DEBUG_AES_CORE
  get_bytes_from_state_array(out, my_bytes);
  printf("\nOutput :\n");
  print_bytes(my_bytes, C * R);
#endif
}

  
#endif



#ifdef DEBUG_AES_CORE
int main(void) {

  word *plaintext;
  plaintext = (word*)malloc(C * sizeof(word));
  word *ciphertext;
  ciphertext = (word*)malloc(C * sizeof(word));

  plaintext[0] = 0x00112233;
  plaintext[1] = 0x44556677;
  plaintext[2] = 0x8899aabb;
  plaintext[3] = 0xccddeeff;

  word *user_key;
  user_key = (word*)malloc(C * sizeof(word));

  user_key[0] = 0x00010203;
  user_key[1] = 0x04050607;
  user_key[2] = 0x08090a0b;
  user_key[3] = 0x0c0d0e0f;

  byte *userKey;
  userKey = (byte*)malloc(C * R * sizeof(byte));

  get_bytes_from_state_array(user_key, userKey);

  AES_KEY *key;
  key = (AES_KEY*)malloc(sizeof(AES_KEY));
  
  init_AES_KEY(10, key);
  set_round_keys(userKey, key);

  aes_rounds(plaintext, ciphertext, key, 1, 1);

  free_AES_KEY(key);
  free(plaintext);
  free(ciphertext);
  free(user_key);
  free(userKey);

  return 0;
}

#endif



  
