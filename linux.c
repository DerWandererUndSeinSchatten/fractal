#include "program.h"

/*
static
u32 iter (u32 Y, u32 X, u32 D);
*/

#include <math.h>
#include <immintrin.h>

#define CALC_ITERATIONS 0x80

nil calc (u32 Y, u32 X, u32 D, img A, u32 L)
{
  f64 d = 4.0f;
  d = d / D;
  f64 x = d - (d * (D / 2) + .5f);
  x = x + X * d;
  f64 y = d - (d * (D / 2));
  y = y + Y * d;

  __m256d _P = _mm256_setzero_pd ();
  __m256d _Q = _mm256_setzero_pd ();  
  __m256d _R = _mm256_set_pd (x + (3 * d), x + (2 * d), x + d, x);
  __m256d _S = _mm256_set1_pd (y);
  __m256d _U = _mm256_setzero_pd ();
  __m256d _V = _mm256_setzero_pd ();
  __m256d _W = _mm256_setzero_pd ();
  __m256d _X = _mm256_setzero_pd ();
  __m256d _Y = _mm256_setzero_pd ();
  __m256d _A = _mm256_setzero_pd ();
  __m256d _B = _mm256_setzero_pd ();
  __m256d _C = _mm256_setzero_pd ();
  __m256d _D = _mm256_setzero_pd ();
  
  for (int i = 0; i < CALC_ITERATIONS; i++) {

    _U = _mm256_mul_pd (_P, _P);
    _V = _mm256_mul_pd (_Q, _Q);
    _W = _mm256_mul_pd (_P, _Q);    
    _X = _mm256_sub_pd (_U, _V);
    _Y = _mm256_add_pd (_W, _W);
    _P = _mm256_add_pd (_X, _R);
    _Q = _mm256_add_pd (_Y, _S);
    _A = _mm256_mul_pd (_P, _P);
    _B = _mm256_mul_pd (_Q, _Q);
    _C = _mm256_add_pd (_A, _B);

    for (u32 j = 0; j < L; j++) {
      ((u64*)&_D)[j] = sqrt (((f64*)&_C)[j]) < 2.0 ? i * 2 : 0;
    }
  }
  
  for (int i = 0; i < L; i++) {
    A [i] = 0xFF000000 + (((u64*)&_D)[i] << 8);
  }  
}

//#define CALC_ITERATIONS 0x40
/*
static u32 iter (u32 Y, u32 X, u32 D)
{
  u32 r = 0;
  f64 x = 0;
  f64 y = 0;
  f64 d = 0;
  c64 Z = 0;
  c64 z = 0;
  c64 C = 0;
  u32 i = 0;
  
  if (! D) {
    return 0xFF000000;
  }

  d = 4.f;
  d = d / D;
  x = d - (d * (D / 2) + .5f);
  x = x + X * d;
  y = d - (d * (D / 2));
  y = y + Y * d;
  C = x + y * I;
  
  Z = 0 + 0 * I;
  z = Z;

  for (i = 0; i < CALC_ITERATIONS; i++) {
    z = Z * Z;
    z = z + C;
    if (cabs (z) > 2.0) {
      break;
    }
    Z = z;    
  }

  r = 0xFF000000 + i * 4;

  return r;
}
*/
