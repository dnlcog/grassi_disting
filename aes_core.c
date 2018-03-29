/* aes_core.c */


/*
This file contains functions computing the AES rounds in different ways.
They have been implemented to be called many times and to be as fast and
simple as possible.
Only implemented for C = 4 and R = 4 for the moment.
*/


#include "aes.h"


/*
This preprocessor variable was created for debugging purposes.
*/
/*
#define DEBUG_AES_CORE
*/

#if (C == 4) && (R == 4)

void aes_rounds(const word32 *in, word32 *out, const AES_KEY *key, const int last_round_without_mixcolumns, int init, int clear) {

  static word32 *s;
  static word32 *t;
  static int r;
  static int i = 0;
  static const word32 *rk;

  /*
  We allocate memory for the very first call of the function.
  s and t are itermediate states of the state array.
  */
  if(init) {
    t = (word32*)malloc(C * sizeof(word32));
    s = (word32*)malloc(C * sizeof(word32));
  }

  
  r = key->rounds;
  rk = key->rd_key;


#ifdef DEBUG_AES_CORE
  byte8 *my_bytes = (byte8*)malloc(C * R * sizeof(byte8));
  get_bytes_from_state_array(rk, my_bytes);
  printf("\nThe key is :\n");
  print_bytes(my_bytes, C * R);
  
  get_bytes_from_state_array(in, my_bytes);
  printf("\nThe input is :\n");
  print_bytes(my_bytes, C * R);
#endif

  /* First round key addition. */
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

    /* If all the rounds are done... */
    if(r == 0) {
      for(i = 0 ; i < C ; i++)
	out[i] = s[i];
      rk += C;
      break;
    }

    /* Round for t (odd rounds). */
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



    /* If all the rounds are done... */
    if(r == 0) {
      for(i = 0 ; i < C ; i++)
	out[i] = t[i];  
      break;
    }
  
    /* Round for s (even rounds). */
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


  /* Last round without the mix columns step. */
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


  /* 
  Clear the intermediate state arrays at the last
  call of the function.
  */
  if(clear) {
    free(t);
    free(s);
  }

#ifdef DEBUG_AES_CORE
  get_bytes_from_state_array(out, my_bytes);
  printf("\nOutput :\n");
  print_bytes(my_bytes, C * R);
  free(my_bytes);
#endif
}


#endif





  
