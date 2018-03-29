/* aes_test.c */



/*
This file aims to test if this implementation of the AES
is correct.
The test vectors are taken from the NIST's FIPS 197.
The parameters of param_and_types.h must be set to :
R = C = K = 4
E = 8
*/



#include "aes.h"


int main(void) {

  word32 *plaintext;
  word32 *ciphertext;
  word32 *user_key;
  byte8 *userKey;
  AES_KEY *key;

  byte8 *printing_bytes;

  plaintext = (word32*)malloc(C * sizeof(word32));
  ciphertext = (word32*)malloc(C * sizeof(word32));

  user_key = (word32*)malloc(C * sizeof(word32));
  userKey = (byte8*)malloc(C * R * sizeof(byte8));
  printing_bytes = (byte8*)malloc(C * R * sizeof(byte8));

  key = (AES_KEY*)malloc(sizeof(AES_KEY));
  init_AES_KEY(10, key);



  plaintext[0] = 0x00112233;
  plaintext[1] = 0x44556677;
  plaintext[2] = 0x8899aabb;
  plaintext[3] = 0xccddeeff;

  user_key[0] = 0x00010203;
  user_key[1] = 0x04050607;
  user_key[2] = 0x08090a0b;
  user_key[3] = 0x0c0d0e0f;

  get_bytes_from_state_array(user_key, userKey);
  set_round_keys_rijndael(userKey, key);



  aes_rounds(plaintext, ciphertext, key, 1, 1, 1);



  get_bytes_from_state_array(plaintext, printing_bytes);
  printf("The plaintext is :\n");
  print_bytes(printing_bytes, C * R);
  
  get_bytes_from_state_array(ciphertext, printing_bytes);
  printf("The ciphertext is :\n");
  print_bytes(printing_bytes, C * R);
  

  free_AES_KEY(key);
  free(plaintext);
  free(ciphertext);
  free(user_key);
  free(userKey);

  return 0;
}

