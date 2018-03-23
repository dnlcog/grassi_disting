#include "aes.h"

#if (C == 4) && (R == 4)

void aes_rounds(const word *in, word *out, const AES_KEY *key, const int last_round_without_mixcolumns) {

  word s0, s1, s2, s3, t0, t1, t2, t3;
  int r = key->rounds;
  const word *rk;
  rk = key->rd_key;
  
  s0 = in[0] ^ rk[0];
  s1 = in[1] ^ rk[1];
  s2 = in[2] ^ rk[2];
  s3 = in[3] ^ rk[3];

  if(last_round_without_mixcolumns)
    r--;

  for (;;) {
    t0 =
      Te0[(s0 >> 24)       ] ^
      Te1[(s1 >> 16) & 0xff] ^
      Te2[(s2 >>  8) & 0xff] ^
      Te3[(s3      ) & 0xff] ^
      rk[4];
    t1 =
      Te0[(s1 >> 24)       ] ^
      Te1[(s2 >> 16) & 0xff] ^
      Te2[(s3 >>  8) & 0xff] ^
      Te3[(s0      ) & 0xff] ^
      rk[5];
    t2 =
      Te0[(s2 >> 24)       ] ^
      Te1[(s3 >> 16) & 0xff] ^
      Te2[(s0 >>  8) & 0xff] ^
      Te3[(s1      ) & 0xff] ^
      rk[6];
    t3 =
      Te0[(s3 >> 24)       ] ^
      Te1[(s0 >> 16) & 0xff] ^
      Te2[(s1 >>  8) & 0xff] ^
      Te3[(s2      ) & 0xff] ^
      rk[7];

    r--;

    if(r == 0) {
      out[0] = t0;
      out[1] = t1;
      out[2] = t2;
      out[3] = t3;
      break;
    }
  
    rk += 8;

    s0 =
      Te0[(t0 >> 24)       ] ^
      Te1[(t1 >> 16) & 0xff] ^
      Te2[(t2 >>  8) & 0xff] ^
      Te3[(t3      ) & 0xff] ^
      rk[0];
    s1 =
      Te0[(t1 >> 24)       ] ^
      Te1[(t2 >> 16) & 0xff] ^
      Te2[(t3 >>  8) & 0xff] ^
      Te3[(t0      ) & 0xff] ^
      rk[1];
    s2 =
      Te0[(t2 >> 24)       ] ^
      Te1[(t3 >> 16) & 0xff] ^
      Te2[(t0 >>  8) & 0xff] ^
      Te3[(t1      ) & 0xff] ^
      rk[2];
    s3 =
      Te0[(t3 >> 24)       ] ^
      Te1[(t0 >> 16) & 0xff] ^
      Te2[(t1 >>  8) & 0xff] ^
      Te3[(t2      ) & 0xff] ^
      rk[3];

    r--;

    if(r == 0) {
      out[0] = s0;
      out[1] = s1;
      out[2] = s2;
      out[3] = s3;
      break;
    }
  }

  if(last_round_without_mixcolumns) {
    s0 =
      (Te2[(out[0] >> 24)       ] & 0xff000000) ^
      (Te3[(out[1] >> 16) & 0xff] & 0x00ff0000) ^
      (Te0[(out[2] >>  8) & 0xff] & 0x0000ff00) ^
      (Te1[(out[3]      ) & 0xff] & 0x000000ff) ^
      rk[0];

    s1 =
      (Te2[(out[1] >> 24)       ] & 0xff000000) ^
      (Te3[(out[2] >> 16) & 0xff] & 0x00ff0000) ^
      (Te0[(out[3] >>  8) & 0xff] & 0x0000ff00) ^
      (Te1[(out[0]      ) & 0xff] & 0x000000ff) ^
      rk[1];

    s2 =
      (Te2[(out[2] >> 24)       ] & 0xff000000) ^
      (Te3[(out[3] >> 16) & 0xff] & 0x00ff0000) ^
      (Te0[(out[0] >>  8) & 0xff] & 0x0000ff00) ^
      (Te1[(out[1]      ) & 0xff] & 0x000000ff) ^
      rk[2];

    s3 =
      (Te2[(out[3] >> 24)       ] & 0xff000000) ^
      (Te3[(out[0] >> 16) & 0xff] & 0x00ff0000) ^
      (Te0[(out[1] >>  8) & 0xff] & 0x0000ff00) ^
      (Te1[(out[2]      ) & 0xff] & 0x000000ff) ^
      rk[3];

    out[0] = s0;
    out[1] = s1;
    out[2] = s2;
    out[3] = s3;
  }

}

  
#endif


