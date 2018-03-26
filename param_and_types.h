#ifndef PATY
#define PATY

#include <stdio.h>
#include <stdlib.h>

/* Do not touch R and C except for tables_generator.c */
#define R 4
#define C 4

#define E 4
#define SBOX rijndael
#define MIXC rijndael

#define ROUNDKEYS rijndael
/*#define DEBUG_AES_CORE*/

typedef unsigned int word;

struct aes_key_st {
  word *rd_key;
  int rounds;
};

typedef struct aes_key_st AES_KEY;

typedef unsigned char byte;

typedef unsigned long int big_int;

#endif

