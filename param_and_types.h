/* param_and_types.h */



#ifndef PATY
#define PATY

#include <stdio.h>
#include <stdlib.h>

/*
R is the number of rows in a state array.
C is the number of columns in a state array.
K is the number of R bytes words in the master key.
 */
#define R 4
#define C 4
#define K 4


/*
The entries of a state array are defined over
an extension of GF(2) of degree E.
E can take values 4 and 8.
*/
#define E 8


/*
SBOX is the choice for the Sbox among :
  rijndael
MIXC is the choice for the Mixcolumns among :
  rijndael
 */
#define SBOX rijndael
#define MIXC rijndael


typedef unsigned int word32;

struct aes_key_st {
  word32 *rd_key;
  int rounds;
};

typedef struct aes_key_st AES_KEY;

typedef unsigned char byte8;

typedef unsigned long int int64;

#endif

