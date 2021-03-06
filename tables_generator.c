/* tables_generator.c */

/* Write in the file compact_tables.h the encryption tables for the chosen parameters. */
/* The encryption tables are sbox, mix_col and, for i in [0, R-1] : */
/* Te(i)[x] = Sbox[x].[MC[0][i], MC[1][i], ... , MC[R-1][i]]. */
/* Each element is a row of at most four 8 bits bytes written as a 32 bits integer. */

#include "param_and_types.h"

/* By default, we use the rijndael Sbox and Mixcolumns */
#ifndef SBOX
#define SBOX rijndael
#endif

#ifndef MIXC
#define MIXC rijndael
#endif



byte8 power(byte8, int);
byte8 mult_by_X(byte8);
byte8 mult_bytes(byte8, byte8);
int build_sbox_and_mix_col(byte8 *, byte8 *, byte8 **, byte8 **);




/*
The main function computes and prints the tables in a file.
*/
int main(void) {

  FILE *f_out;

  char *output_file_name = "compact_tables.h";

  /* Create the file. */
  if((f_out = fopen(output_file_name, "w")) == NULL) {
    fprintf(stderr, "\nError: Cannot write into the file %s\n", output_file_name);
    return(EXIT_FAILURE);
  }

  fprintf(f_out, "/* Tables for the AES variant : r = %u, e = %u*/\n\n", R, E);



  
  /* Declare Sbox table and the Mixcolumns matrix. */
  byte8 sbox[1 << E];
  byte8 inv_sbox[1 << E];
  byte8 **mix_col = (byte8**)malloc(R * sizeof(byte8*));
  for(int k = 0 ; k < R ; k++)
    mix_col[k] = (byte8*)malloc(R * sizeof(byte8));
  byte8 **inv_mix_col = (byte8**)malloc(R * sizeof(byte8*));
  for(int k = 0 ; k < R ; k++)
    inv_mix_col[k] = (byte8*)malloc(R * sizeof(byte8));

  /* Fill them according to the choices made in the preprocessor variables. */
  build_sbox_and_mix_col(sbox, inv_sbox, mix_col, inv_mix_col);




  /* Print the sbox table */
  fprintf(f_out, "static const byte8 sbox[%d] = {\n", 1 << E);
  
  /* l is the index of the line of the table k to be written. We choose 4 columns. */
  for(int l = 0 ; l < (1 << (E - 2)) ; l++) {
    fprintf(f_out, "  ");
    
    /* m is the index of the column. */
    for(int m = 0 ; m < 4 ; m++) {
      fprintf(f_out, "0x%xU", sbox[(4 * l) + m]);
      if(((4 * l) + m) != ((1 << E) - 1))
	fprintf(f_out, ", ");
    }

    fprintf(f_out, "\n");
  }

  fprintf(f_out, "};\n\n");



  /* Print the inv_sbox table */
  fprintf(f_out, "static const byte8 inv_sbox[%d] = {\n", 1 << E);
  
  for(int l = 0 ; l < (1 << (E - 2)) ; l++) {
    fprintf(f_out, "  ");
    
    for(int m = 0 ; m < 4 ; m++) {
      fprintf(f_out, "0x%xU", inv_sbox[(4 * l) + m]);
      if(((4 * l) + m) != ((1 << E) - 1))
	fprintf(f_out, ", ");
    }

    fprintf(f_out, "\n");
  }

  fprintf(f_out, "};\n\n");



  /* Print the mix_columns table. */
  fprintf(f_out, "static const byte8 mix_col[%d][%d] = {\n", R, R);

  for(int i = 0 ; i < R ; i++) {
    fprintf(f_out, "  {");
    for(int j = 0 ; j < R ; j++) {
      fprintf(f_out, "0x%xU", mix_col[i][j]);
      if(j != (R - 1))
	fprintf(f_out, ", ");
      else
	fprintf(f_out, "}");
    }
    if(i != (R - 1))
      fprintf(f_out, ",");
    fprintf(f_out, "\n");
  }
  fprintf(f_out, "};\n\n");
  




  /* Print the inv_mix_columns table. */
  fprintf(f_out, "static const byte8 inv_mix_col[%d][%d] = {\n", R, R);

  for(int i = 0 ; i < R ; i++) {
    fprintf(f_out, "  {");
    for(int j = 0 ; j < R ; j++) {
      fprintf(f_out, "0x%xU", inv_mix_col[i][j]);
      if(j != (R - 1))
	fprintf(f_out, ", ");
      else
	fprintf(f_out, "}");
    }
    if(i != (R - 1))
      fprintf(f_out, ",");
    fprintf(f_out, "\n");
  }
  fprintf(f_out, "};\n\n");
  




  /* Print the Te_ tables. */
  byte8 temp;
  word32 to_print;

  /* k is the index of the encryption table to be written in the file. */
  for(int k = 0 ; k < R ; k++) {

    fprintf(f_out, "static const word32 Te%d[%d] = {\n", k, 1 << E);

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






  /* Print the Td_ tables. */

  for(int k = 0 ; k < R ; k++) {

    fprintf(f_out, "static const word32 Td%d[%d] = {\n", k, 1 << E);

    for(int l = 0 ; l < (1 << (E - 2)) ; l++) {
      fprintf(f_out, "  ");

      for(int m = 0 ; m < 4 ; m++) {

	temp = inv_sbox[(4 * l) + m];

	to_print = 0x00000000U;
	for(int i = 0 ; i < R ; i++)
	  to_print ^= (mult_bytes(temp, inv_mix_col[i][k]) << (2 * 4 * (R - i - 1)));
	
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

  for(int k = 0 ; k < R ; k++)
    free(inv_mix_col[k]);
  free(inv_mix_col);

  fclose(f_out);
  return(EXIT_SUCCESS);

}

/* Fill the tables sbox and mix_col according to the preprocessor variables SBOX and MIXC. */
int build_sbox_and_mix_col(byte8 *sbox, byte8 *inv_sbox, byte8 *mix_col[], byte8 *inv_mix_col[]) {

#if SBOX == rijndael

  byte8 i = 0;

  /* Inversion in the finite field. */
  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte8)j;
    sbox[i] = power(i, (1 << E) - 2);
  }

  /* Affine map. */
#if E == 4

  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte8)j;
    sbox[i] = 0x0f & (sbox[i] ^ 0x06 ^
      (sbox[i] >> 1) ^ (sbox[i] << 3) ^
      (sbox[i] >> 2) ^ (sbox[i] << 2));
  }

#elif E == 8

  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte8)j;
    sbox[i] = sbox[i] ^ 0x63 ^
      (sbox[i] >> 4) ^ (sbox[i] << 4) ^
      (sbox[i] >> 5) ^ (sbox[i] << 3) ^
      (sbox[i] >> 6) ^ (sbox[i] << 2) ^
      (sbox[i] >> 7) ^ (sbox[i] << 1);
  }
  
#endif

#endif

  /* Inverse sbox. */
  for(int j = 0 ; j < (1 << E) ; j++) {
    i = (byte8)j;
    for(int k = 0 ; k < (1 << E) ; k++) {
      if(sbox[k] == i) {
	inv_sbox[i] = (byte8)k;
	break;
      }
    }
  }
    

#if MIXC == rijndael

#if R == 1
  byte8 mix_col_local[R][R] = {{0x01}};
  byte8 inv_mix_col_local[R][R] = {{0x01}};

#elif R == 2
  byte8 mix_col_local[R][R] = {{0x03, 0x02},
			{0x02, 0x03}};
  byte8 inv_mix_col_local[R][R] = {{0x03, 0x02},
			{0x02, 0x03}};

#elif R == 4
  byte8 mix_col_local[R][R] = {{0x02, 0x03, 0x01, 0x01},
			{0x01, 0x02, 0x03, 0x01},
			{0x01, 0x01, 0x02, 0x03},
			{0x03, 0x01, 0x01, 0x02}};
  byte8 inv_mix_col_local[R][R] = {{0x0e, 0x0b, 0x0d, 0x09},
			{0x09, 0x0e, 0x0b, 0x0d},
			{0x0d, 0x09, 0x0e, 0x0b},
			{0x0b, 0x0d, 0x09, 0x0e}};
#endif

  for(int i = 0 ; i < R ; i++)
    for(int j = 0 ; j < R ; j++)
      mix_col[i][j] = mix_col_local[i][j];
  for(int i = 0 ; i < R ; i++)
    for(int j = 0 ; j < R ; j++)
      inv_mix_col[i][j] = inv_mix_col_local[i][j];
  
#endif

  return(EXIT_SUCCESS);

}

/* Computes x^n in the finite field. */
byte8 power(byte8 x, int n) {

  byte8 y = 0x01;

  for(int i = 0 ; i < n ; i++)
    y = mult_bytes(y, x);

  return y;
}

/* computes b times X where X is a root of the primitive polynomial. */
byte8 mult_by_X(byte8 b) {

#if E == 4
  byte8 b3 = (b >> 3) & 0x01;
  byte8 c = (b << 1) & 0x0f;
  return (c ^ (b3 << 1) ^ b3);

#elif E == 8
  byte8 b7 = (b >> 7) & 0x01;
  byte8 c = b << 1;
  return (c ^ (b7 << 4) ^ (b7 << 3) ^ (b7 << 1) ^ b7);

#endif
}

/* Computes a*b in the finite field. */
byte8 mult_bytes(byte8 a, byte8 b) {

  byte8 c = 0;
  byte8 temp = a;

  for(int i = 0 ; i < E ; i++) {
    if((b >> i) & 0x01) {
      c ^= temp;
    }
    temp = mult_by_X(temp);
  }

  return c;
}


