#include "program.h"

#include <math.h>
#include <immintrin.h>

#define CALC_ITERATION 0x64
#define CALC_THRESHOLD 2.

nil calc (u32 Y, u32 X, u32 D, img A, u32 L)
{
  f64 d = 4.0f;
  d = d / D;
  f64 x = d - (d * (D / 2) + .5f);
  x = x + X * d;
  f64 y = d - (d * (D / 2));
  y = y + Y * d;

  __m256 _P = _mm256_setzero_ps ();
  __m256 _Q = _mm256_setzero_ps ();  
  __m256 _R = _mm256_set_ps (x + (7 * d), x + (6 * d),
			      x + (5 * d), x + (4 * d),
			      x + (3 * d), x + (2 * d),
			      x + (1 * d), x + (0 * d));
  __m256 _S = _mm256_set1_ps (y);
  __m256 _U = _mm256_setzero_ps ();
  __m256 _V = _mm256_setzero_ps ();
  __m256 _W = _mm256_setzero_ps ();
  __m256 _X = _mm256_setzero_ps ();
  __m256 _Y = _mm256_setzero_ps ();
  __m256 _A = _mm256_setzero_ps ();
  __m256 _B = _mm256_setzero_ps ();
  __m256 _C = _mm256_setzero_ps ();
  __m256 _D = _mm256_setzero_ps ();
  
  for (int i = 0; i < CALC_ITERATION; i++) {

    _U = _mm256_mul_ps (_P, _P);
    _V = _mm256_mul_ps (_Q, _Q);
    _W = _mm256_mul_ps (_P, _Q);    
    _X = _mm256_sub_ps (_U, _V);
    _Y = _mm256_add_ps (_W, _W);
    _P = _mm256_add_ps (_X, _R);
    _Q = _mm256_add_ps (_Y, _S);
    _A = _mm256_mul_ps (_P, _P);
    _B = _mm256_mul_ps (_Q, _Q);
    _C = _mm256_add_ps (_A, _B);

    for (u32 j = 0; j < L; j++) {
      if (abs (((f32*)&_C)[j]) < CALC_THRESHOLD) {
	((u32*)&_D)[j] = i * 0x20;
      }
    }
    
  }

  for (int i = 0; i < L; i++) {
    A [i] = 0xFF000000 + ((u32*)&_D)[i];
  }
}
