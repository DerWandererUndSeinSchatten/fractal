#include "program.h"

//typedef struct {} linux_t;

#include <math.h>
#include <unistd.h>
#include <immintrin.h>

s32 f_offset_z = 0; // zoom
s32 f_offset_h = 0; // move, horizontal
s32 f_offset_v = 0; // move, vertical

static nil cmd_wildcard ();
static nil cmd_scroll_n ();
static nil cmd_scroll_e ();
static nil cmd_scroll_s ();
static nil cmd_scroll_w ();
static nil cmd_zoom_inc ();
static nil cmd_zoom_dec ();
static nil cmd_loop_end ();

static nil (*f_cmd [COMMANDS]) () = 
{
  cmd_wildcard, cmd_scroll_n, cmd_scroll_e, cmd_scroll_s,
  cmd_scroll_w, cmd_zoom_inc, cmd_zoom_dec, cmd_loop_end
};

nil gen_conf (u16 F)
{

  if (F >= COMMANDS) {
    return;
  }
  
  f_cmd [F] ();
  
}

#define CALC_ITERATION 0x64
#define CALC_THRESHOLD 2.
#define CALC_OPTIMIZED

nil gen_calc (u32 Y, u32 X, u32 D, img A, u32 L)
{
  f64 d = 4.0f;
  d = d / D;
  f64 x = d - (d * (D / 2) + .5f);
  x = x + X * d;
  f64 y = d - (d * (D / 2));
  y = y + Y * d;

  __m256 _P = _mm256_setzero_ps ();
  __m256 _Q = _mm256_setzero_ps ();  

  __m256 _R =
    _mm256_setr_ps (x + (0 * d), x + (1 * d), x + (2 * d), x + (3 * d),
		    x + (4 * d), x + (5 * d), x + (6 * d), x + (7 * d));
  
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

#ifdef CALC_OPTIMIZED
  __m256 _E = _mm256_setzero_ps ();
  __m256 _F = _mm256_setzero_ps ();
  __m256 _H = _mm256_set1_ps (0x40);
  __m256 _G = _mm256_setzero_ps ();
#endif
  
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

#ifdef CALC_OPTIMIZED 
    
    _D = _mm256_setr_ps
      ((abs (((f32*)&_C)[0]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[1]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[2]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[3]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[4]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[5]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[6]) < CALC_THRESHOLD),
       (abs (((f32*)&_C)[7]) < CALC_THRESHOLD));
    
    _E = _mm256_set1_ps ((f32)i);
    _E = _mm256_mul_ps (_E, _H);
    _F = _mm256_mul_ps (_D, _E);
    _G = _mm256_add_ps (_F, _G);
    
#else

    for (int j = 0; j < L; j++) {
      ((u32*)&_D)[j] += (abs (((f32*)&_C)[j]) < CALC_THRESHOLD) * i * 0x40;
    }
    
#endif

  }

#ifdef CALC_OPTIMIZED

  A [0] = 0xFF000000 + ((f32*)&_G)[0];
  A [1] = 0xFF000000 + ((f32*)&_G)[1];
  A [2] = 0xFF000000 + ((f32*)&_G)[2];
  A [3] = 0xFF000000 + ((f32*)&_G)[3];
  A [4] = 0xFF000000 + ((f32*)&_G)[4];
  A [5] = 0xFF000000 + ((f32*)&_G)[5];
  A [6] = 0xFF000000 + ((f32*)&_G)[6];
  A [7] = 0xFF000000 + ((f32*)&_G)[7];

#else

  for (int i = 0; i < L; i++) {
    A [i] = 0xFF000000 + ((s32*)&_D)[i];
  }
    
#endif
  
}

static nil cmd_wildcard ()
{
  
}

static nil cmd_scroll_n ()
{
  
}

static nil cmd_scroll_e ()
{
  
}

static nil cmd_scroll_s ()
{
  
}

static nil cmd_scroll_w ()
{
  
}

static nil cmd_zoom_inc ()
{
  
}

static nil cmd_zoom_dec ()
{
  
}

#include <signal.h>

static nil cmd_loop_end ()
{
  raise (SIGINT);
}
