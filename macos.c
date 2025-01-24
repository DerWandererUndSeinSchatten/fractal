#include "program.h"

static
u32 iter (u32 Y, u32 X, u32 D);
  
nil calc (u32 Y, u32 X, u32 D, img A, u32 L)
{
  for (u32 i = 0; i < L; i++) {
    A [i] = iter (Y, X + i, D);
  }
}

#include <math.h>

#define CALC_ITERATIONS 0x40

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
