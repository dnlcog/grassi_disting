#ifndef PATY
#define PATY

#include <stdio.h>
#include <stdlib.h>

#define N 5
#define R 4
#define C 4
#define E 4

#define ROUNDKEYS_MODE random

typedef unsigned int word;

struct aes_key_st {
  word *rd_key;
  int rounds;
};

typedef struct aes_key_st AES_KEY;

typedef unsigned char byte;

typedef unsigned long int big_int;

#endif

