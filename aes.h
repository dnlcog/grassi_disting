#ifndef AES_H
#define AES_H

#include "param_and_types.h"
#include "compact_tables.h"


/* key_schedule.c functions */

int set_round_keys(const byte *, AES_KEY *);
void init_AES_KEY(const int, AES_KEY *);
void free_AES_KEY(AES_KEY *);



/* aes_core.c functions */

#if (C == 4) && (R == 4)
void aes_rounds(const word *, word *, const AES_KEY *, int);
#endif




/* utilities.c functions */

void get_word_from_bytes(const byte *, word *);
void get_bytes_from_word(const word, byte *);
void print_state_array(word *);

#endif
