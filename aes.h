/* aes.h */


/*
This file is a common header for the files :
key_schedule.c
aes_core.c
utilities.c
*/


#ifndef AES_H
#define AES_H

#include "param_and_types.h"
#include "compact_tables.h"


/* key_schedule.c functions */

int set_round_keys_rijndael(const byte8 *, AES_KEY *);
int set_round_keys_random(AES_KEY *);
void init_AES_KEY(const int, AES_KEY *);
void free_AES_KEY(AES_KEY *);



/* aes_core.c functions */

#if (C == 4) && (R == 4)
void aes_rounds(const word32 *, word32 *, const AES_KEY *, int, int, int);
#endif




/* utilities.c functions */

void get_word_from_bytes(const byte8 *, word32 *);
void get_bytes_from_word(const word32, byte8 *);
void print_state_array(word32 *);
void print_bytes(byte8 *, unsigned int);
void get_state_array_from_bytes(const byte8 *, word32 *);
void get_bytes_from_state_array(const word32 *, byte8 *);
void next_plaintext(word32 *, const int);


#endif
