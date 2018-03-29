/* distinguisher.c */


/*
This program computes the number of pairs of plaintexts
in the same coset of Di such that their ciphertexts
after 5 rounds belong to the same coset of M_[0;3]\\j,
for all i and j.

It writes the results in a .csv file.
*/


#include "aes.h"




void next_plaintext(word32 *, const int);

int count_5_rounds_collisions(const int, const int, const AES_KEY *, int64 *);



/* Write the number of pairs in result, given i, j and an expanded key. */
int count_5_rounds_collisions(const int i, const int j, const AES_KEY *key, int64 *result) {

  if(key->rounds != 5) {
    fprintf(stderr, "\nError : the number of rounds in the key schedule should be 5 here. \n\n");
    return(EXIT_FAILURE);
  }

  /* bound is the size of the counter array. */
  int64 bound = (int64)(1) << ((C * E) - 3);
  
  int64 nb_plaintexts = (int64)(1) << (C * E);
  int64 k;
  int64 l;

  /*
  The counter array has one 8 bits byte for each possible ciphertext.
  Those 8 bits bytes are stored by 8 in a 64 bits integer.
  */
  int64 *counter_array = (int64*)calloc(bound, sizeof(int64));

  word32 *plaintext = (word32*)calloc(C, sizeof(word32));
  word32 *ciphertext = (word32*)calloc(C, sizeof(word32));

  word32 x;
  int m;

  word32 c;

  byte8 temp;

  /* For each plaintext... */
  for(k = 0 ; k < nb_plaintexts ; k++) {

    /* Compute the ciphertext, */
    aes_rounds(plaintext, ciphertext, key, 1, (k == 0), (k == (nb_plaintexts - 1)));

    /* its corresponding index, */
    x = 0;
    for(m = 0 ; m < C ; m++) {
      c = (ciphertext[(C + j - m) % C] >> (8 * (R - m - 1))) & 0xff;
      x ^= c << (m * E);
    }

    /* and increment the value of counter array at this index. */
    counter_array[x >> 3] += ((int64)(1) << ((x & 0xfff) << 3));

    
    next_plaintext(plaintext, i);
  }

  /* Now that the counter array is filled, count the pairs. */
  *result = 0; 
  for(k = 0 ; k < bound ; k++) {
    for(l = 0 ; l < 8 ; l++) {
      temp = (byte8)((counter_array[k] >> (l << 3)) & 0xff);
      *result += ((temp * (temp - 1)) >> 1);
    }
  }


  /* What follows just gets some statistics about the collisions. */
  int64 count0 = 0;
  int64 count1 = 0;
  byte8 max = counter_array[0] & 0xff;
  byte8 min = counter_array[0] & 0xff;
  for(k = 0 ; k < bound ; k++) {
    for(l = 0 ; l < 8 ; l++) {
      temp = (byte8)((counter_array[k] >> (l << 3)) & 0xff);
      if(temp < min)
	min = temp;
      if(temp > max)
	max = temp;
      if(temp == 0)
	count0++;
      if(temp == 1)
	count1++;
    }
  }

  printf("%d : %d, %lf zeros, %lf ones, %lf rest\n",
	 (int)min, (int)max,
	 (double)(count0 * 100) / nb_plaintexts,
	 (double)(count1 * 100) / nb_plaintexts,
	 (double)((nb_plaintexts - count1 - count0) * 100) / nb_plaintexts
	);
    
  free(counter_array);
  free(plaintext);
  free(ciphertext);

  return(EXIT_SUCCESS);
}



/* Rewrite the state array with its next plaintext. */
void next_plaintext(word32 *state_array, const int i) {

  int k = 0;
  word32 temp;
  
  while(k < R) {
    temp = ((state_array[(i + k) % C] >> (8 * (R - k - 1))) + 1) & 0xff;
#if E == 4
    temp = temp & 0x0f;
#endif
    state_array[(i + k) % C] = temp << (8 * (R - k - 1));
    if(temp == 0)
      k++;
    else
      k = R;
  }
}




/*
The main function basically calls count_5_rounds_collisions
for all i and j and prints the results in a file.
*/
int main(void) {

  AES_KEY *key = (AES_KEY*)malloc(sizeof(AES_KEY));
  byte8 *useless = (byte8*)malloc(sizeof(byte8));

  init_AES_KEY(5, key);
  set_round_keys_random(key);

  FILE *f_out;

  char file_name[20];
  sprintf(file_name, "all_n_for_E_%d.csv", E);

  if((f_out = fopen(file_name, "w")) == NULL) {
    fprintf(stderr, "\nError: Cannot write into the file %s\n", file_name);
    return(EXIT_FAILURE);
  }

  int64 **result = (int64**)malloc(C * sizeof(int64*));

  fprintf(f_out, "i\\j,");
  
  for(int i = 0 ; i < C ; i++) {
    fprintf(f_out, "%d", i+1);
    if(i == (C - 1))
      fprintf(f_out, "\n");
    else
      fprintf(f_out, ",");
  }

  for(int i = 0 ; i < C ; i++) {
    fprintf(f_out, "%d,", i+1);
    result[i] = (int64*)malloc(C * sizeof(int64));
    for(int j = 0 ; j < C ; j++) {
      count_5_rounds_collisions(i, j, key, result[i] + j);
      fprintf(f_out, "%lu", result[i][j]);
      if(j == (C - 1))
	fprintf(f_out, "\n");
      else
	fprintf(f_out, ",");
    }
  }
  fprintf(f_out, "\n");
  fclose(f_out);
  

  printf("\nFull\n");

  printf("i\\j    ");

  for(int i = 0 ; i < C ; i++)
    printf("%-10d ", i+1);
  printf("\n");
  
  for(int i = 0 ; i < C ; i++) {
    printf("%d ", i+1);
    for(int j = 0 ; j < C ; j++)
      printf("%10lu ", result[i][j]);
    printf("\n");
  }
  
  printf("\nMod 8\n");

  printf("i\\j    ");

  for(int i = 0 ; i < C ; i++)
    printf("%-10d ", i+1);
  printf("\n");
  
  for(int i = 0 ; i < C ; i++) {
    printf("%d ", i+1);
    for(int j = 0 ; j < C ; j++)
      printf("%10lu ", result[i][j] % 8);
    printf("\n");
  }
  
  printf("\nDiv by  8\n");

  printf("i\\j    ");

  for(int i = 0 ; i < C ; i++)
    printf("%-10d ", i+1);
  printf("\n");
  
  for(int i = 0 ; i < C ; i++) {
    printf("%d ", i+1);
    for(int j = 0 ; j < C ; j++)
      printf("%10lu ", result[i][j] / 8);
    printf("\n");
  }
  
  
  for(int i = 0 ; i < C ; i++)
    free(result[i]);
  free(result);
  free_AES_KEY(key);
  free(useless);

  return(EXIT_SUCCESS);
}



