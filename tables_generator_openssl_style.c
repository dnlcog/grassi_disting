/* tables_generator.c */

/* Write in the file compact_tables_R_E.h the encryption tables for the chosen parameters. */
/* The encryption tables are, for i in [0, R-1] : */
/* Te(i)[x] = Sbox[x].[MC[0][i], MC[1][i], ... , MC[R-1][i]]. */
/* Each element is a row of at most four bytes written as a 4bytes integer. */

#include "aes.h"

/* By default, we use the rijndael Sbox and Mixcolumns */
#ifndef SBOX
#define SBOX rijndael
#endif

#ifndef MIXC
#define MIXC rijndael
#endif

byte power(byte, int);
byte mult_by_X(byte);
byte mult_bytes(byte, byte);
int build_sbox_and_mix_col(byte *, byte **);

int main(void) {

  FILE *f_out;

  /* Format the name of the output file. */
  char output_file_name[100];
  sprintf(output_file_name, "compact_tables_R%u_E%u.h", R, E);

  /* Test whether the file already exists or not. */
  if((f_out = fopen(output_file_name, "r")) != NULL) {
    fprintf(stdout, "\nFile %s already exists\n", output_file_name);
    fclose(f_out);
    return(EXIT_SUCCESS);
  }

  /* If not, create the file. */
  if((f_out = fopen(output_file_name, "w")) == NULL) {
    fprintf(stderr, "\nError: Cannot write into the file %s\n", output_file_name);
    return(EXIT_FAILURE);
  }

  fprintf(f_out, "/* Tables for the AES variant : r = %u, e = %u*/\n\n", R, E);

  /* Declare Sbox table and the Mixcolumns matrix. */
  byte sbox[1 << E];
  byte **mix_col = (byte**)malloc(R * sizeof(byte*));
  for(int k = 0 ; k < R ; k++)
    mix_col[k] = (byte*)malloc(R * sizeof(byte));

  /* Fill them according to the choices made in the preprocessor variables. */
  build_sbox_and_mix_col(sbox, mix_col);

  byte temp;
  word to_print;

  /* k is the index of the encryption table to be written in the file. */
  for(int k = 0 ; k < R ; k++) {

    fprintf(f_out, "static const word Te%d[%d] = {\n", k, 1 << E);

    /* l is the index of the line of the table k to be written. We choose 4 columns. */
    for(int l = 0 ; l < (1 << (E - 2)) ; l++) {
      fprintf(f_out, "  ");

      /* m is the index of the column. */
      for(int m = 0 ; m < 4 ; m++) {

	/* If x = 4l+m, temp is S(x). */
	temp = sbox[(4 * l) + m];

	/* to_print collects the row S(x).[MC[0][k], ... , MC[R-1][k]] in a 32bits word. */
	to_print = 0x00000000U;
	for(int i = 0 ; i < R ; i++)
	  to_print ^= (mult_bytes(temp, mix_col[i][k]) << (2 * 4 * (R - i - 1)));
	
	fprintf(f_out, "0x%xU", to_print);

	if(((4 * l) + m) != ((1 << E) - 1))
	  fprintf(f_out, ", ");
      }

      fprintf(f_out, "\n");
    }

    fprintf(f_out, "};\n\n");
  }

  for(int k = 0 ; k < R ; k++)
    free(mix_col[k]);
  free(mix_col);

  fclose(f_out);
  return(EXIT_SUCCESS);

}

/* Fill the tables sbox and mix_col according to the preprocessor variables SBOX and MIXC. */
int build_sbox_and_mix_col(byte *sbox, byte *mix_col[]) {

#if SBOX == rijndael

  byte i = 0;

  /* Inversion in the finite field. */
  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte)j;
    sbox[i] = power(i, (1 << E) - 2);
  }

  /* Affine map. */
#if E == 4

  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte)j;
    sbox[i] = 0x0f & (sbox[i] ^ 0x06 ^
      (sbox[i] >> 1) ^ (sbox[i] << 3) ^
      (sbox[i] >> 2) ^ (sbox[i] << 2));
  }

#elif E == 8

  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte)j;
    sbox[i] = sbox[i] ^ 0x63 ^
      (sbox[i] >> 4) ^ (sbox[i] << 4) ^
      (sbox[i] >> 5) ^ (sbox[i] << 3) ^
      (sbox[i] >> 6) ^ (sbox[i] << 2) ^
      (sbox[i] >> 7) ^ (sbox[i] << 1);
  }
  
#endif

#endif

#if MIXC == rijndael

#if R == 1
  byte mix_col_local[R][R] = {{0x01}};

#elif R == 2
  byte mix_col_local[R][R] = {{0x03, 0x02},
			{0x02, 0x03}};

#elif R == 4
  byte mix_col_local[R][R] = {{0x02, 0x03, 0x01, 0x01},
			{0x01, 0x02, 0x03, 0x01},
			{0x01, 0x01, 0x02, 0x03},
			{0x03, 0x01, 0x01, 0x02}};
#endif

  for(int i = 0 ; i < R ; i++)
    for(int j = 0 ; j < R ; j++)
      mix_col[i][j] = mix_col_local[i][j];
  
#endif

  return(EXIT_SUCCESS);

}

/* Computes x^n in the finite field. */
byte power(byte x, int n) {

  byte y = 0x01;

  for(int i = 0 ; i < n ; i++)
    y = mult_bytes(y, x);

  return y;
}

/* computes b times X where X is a root of the primitive polynomial. */
byte mult_by_X(byte b) {

#if E == 4
  byte b3 = (b >> 3) & 0x01;
  byte c = (b << 1) & 0x0f;
  return (c ^ (b3 << 1) ^ b3);

#elif E == 8
  byte b7 = (b >> 7) & 0x01;
  byte c = b << 1;
  return (c ^ (b7 << 4) ^ (b7 << 3) ^ (b7 << 1) ^ b7);

#endif
}

/* Computes a*b in the finite field. */
byte mult_bytes(byte a, byte b) {

  byte c = 0;
  byte temp = a;

  for(int i = 0 ; i < E ; i++) {
    if((b >> i) & 0x01) {
      c ^= temp;
    }
    temp = mult_by_X(temp);
  }

  return c;
}


