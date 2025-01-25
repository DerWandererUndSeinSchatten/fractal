#include <SDL2/SDL.h>

#define PROGRAM_VECTOR_EXT // INTRISICS

#ifndef PROGRAM_H
#define PROGRAM_H

typedef unsigned char 	   u08;
typedef unsigned short 	   u16;
typedef unsigned int 	   u32;
typedef unsigned long long u64;
typedef char 		   s08;
typedef short 		   s16;
typedef int 		   s32;
typedef long long int 	   s64;
typedef float		   f32;
typedef double		   f64;
typedef void	           nil;
typedef char*		   str;
typedef unsigned char*	   buf;
typedef unsigned int*	   img;
typedef void*              ref;
#include <complex.h>

typedef float  complex     c32;
typedef double complex     c64;

#define FRACTAL_FPS	   60
#define FRACTAL_DELAY	   1000 / FRACTAL_FPS
#define FRACTAL_SUCCESS	   0x1
#define FRACTAL_FAILURE	   0x0
#define FRACTAL_PROCESS	   0x8
#define FRACTAL_PIXEL	   0x200
#define FRACTAL_DEPTH	   0x20
#define FRACTAL_SPACE	   FRACTAL_PIXEL * FRACTAL_PIXEL * sizeof (u32)
#define FRACTAL_CHANNELS   2
#define FRACTAL_SEMAPHORES 5
#define FRACTAL_TITLE	   "fractal"

typedef int  		   plt [FRACTAL_PROCESS];		

typedef struct {
  struct {
    u32 (*get) (nil*, str, s08);
    s32 d;
  } key;
  struct {
    u32 (*new) (nil*);
    u32 (*del) (nil*);
    u32 (*inc) (nil*, u32);
    u32 (*dec) (nil*, u32);
    u32 (*set) (nil*, u32);
    u32 (*get) (nil*, u32);
    s32 d;
    u32 n;
    s32 v;
  } sem;
  struct {
    u32 (*get) (nil*);
    u32 (*del) (nil*);
    u32 (*map) (nil*);
    u32 (*rem) (nil*);
    s32 d;
    buf b;
  } shm;
  struct {
    union
    {
      u64 val;
      struct 
      {
	s32 r;
	s32 w;
      } channel;
    } pro;
    union
    {
      u64 val;
      struct 
      {
	s32 r;
	s32 w;
      } channel;
    } con;
  } pip;
  struct {
    SDL_Renderer* render;
    SDL_Window*		window;
    SDL_Surface*	bitmap;
    SDL_Texture*	target;
  } gfx;
} ctx;

#endif
