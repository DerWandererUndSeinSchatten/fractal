#include "program.h"

/*************************************************************************
 *
 * FUN, DEC, LOC: JMP
 * 
 *************************************************************************/

static nil jmp_sig (s32);

/*************************************************************************
 *
 * DEP, DEC, LOC: JMP
 * 
 *************************************************************************/

#include <setjmp.h>

/*************************************************************************
 *
 * VAR, DEC, LOC: JMP
 * 
 *************************************************************************/

static jmp_buf f_jmp;

/*************************************************************************
 *
 * FUN, DEC, LOC: CTX
 * 
 *************************************************************************/

static u32 ctx_new (ctx*);
static u32 ctx_gfx (ctx*);
static u32 ctx_eve (ctx*);
static u32 ctx_end (ctx*);

/*************************************************************************
 *
 * FUN, DEC, LOC: KEY
 * 
 *************************************************************************/

static u32 key_get (nil*, str, s08);

/*************************************************************************
 *
 * FUN, DEC, LOC: SEMAPHORE
 * 
 *************************************************************************/

static u32 sem_new (nil*);
static u32 sem_del (nil*);
static u32 sem_inc (nil*, u32);
static u32 sem_dec (nil*, u32);
static u32 sem_set (nil*, u32);
static u32 sem_get (nil*, u32);

/*************************************************************************
 *
 * FUN, DEC, LOC: SHARED MEMORY
 * 
 *************************************************************************/

static u32 shm_get (nil*);
static u32 shm_del (nil*);
static u32 shm_map (nil*);
static u32 shm_rem (nil*);

/*************************************************************************
 *
 * FUN, DEF, LOC: WORK
 * 
 *************************************************************************/

static s32 work (ctx*);
static s32 loop (ctx*);
static u32 draw (ctx*);

/*************************************************************************
 *
 * FUN, DEP, LOC: MAIN
 * 
 *************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: MAIN
 * 
 *************************************************************************/

#define MAIN_VERSION 'a'

int main (int argc, char ** argv)
{
  s32 r = EXIT_SUCCESS;
  plt t = {0};
  ctx c;

  if (FRACTAL_FAILURE == ctx_new (&c)) {
    goto FAILURE;
  }

  if (FRACTAL_FAILURE == c.key.get (&c, *argv, MAIN_VERSION)) {
    goto FAILURE;
  }
	
  if (FRACTAL_FAILURE == c.shm.get (&c)) {
    goto FAILURE;
  }

  for (u32 i = 0; i < FRACTAL_PROCESS; i++) {
    if (! (t [i] = fork ())) {
      exit (work (&c));
    }
  }

  if (FRACTAL_FAILURE == loop (&c)) {
    printf ("[DEBUG] loop ended\n");
  }

  for (u32 i = 0; i < FRACTAL_PROCESS; i++) {
    if (t [i] != waitpid (t [i], 0, 0)) {
      printf ("[DEBUG] %i failed\n", t [i]);
    }
  }

  goto SUCCESS;

 FAILURE:

  r = EXIT_FAILURE;

  if (errno) {
    printf ("[ERROR] %s\n", strerror (errno));
  }

 SUCCESS:

  if (FRACTAL_FAILURE == ctx_end (&c)) {
    r = EXIT_FAILURE;
  }

  return r;
}


/*************************************************************************
 *
 * FUN, DEF, LOC: CTX
 * 
 *************************************************************************/

static u32 ctx_new (ctx* c)
{

  u32 r = FRACTAL_SUCCESS;

  if (! c) {
    goto FAILURE;
  }

  c->key.get 	= key_get;
  c->sem.new 	= sem_new;
  c->sem.del 	= sem_del;
  c->sem.inc 	= sem_inc;
  c->sem.dec 	= sem_dec;
  c->sem.get 	= sem_get;
  c->sem.set 	= sem_set;
  c->sem.n	= FRACTAL_PROCESS + 1;
  c->sem.d      = 0;
  c->shm.get	= shm_get;
  c->shm.del 	= shm_del;
  c->shm.map 	= shm_map;
  c->shm.rem 	= shm_rem;
  c->shm.d	= 0;
  c->shm.b	= 0;
  c->gfx.render	= 0;
  c->gfx.window = 0;
  c->gfx.bitmap = 0;
  c->gfx.target = 0;

  if (0 > pipe ((s32*)&(c->pip.pro))) {
    goto FAILURE;
  }

  if (0 > pipe ((s32*)&(c->pip.con))) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;

}

static u32 ctx_gfx (ctx* c)
{
  u32 r = FRACTAL_SUCCESS;
  s32 u = 0;
  s32 v = 0;
  s32 w = 0;
  u32 j = 0;
  u32 k = 0;
  u32 l = 0;
  u32 m = 0;
  
  if (! c) {
    goto FAILURE;
  }

  if (0 > SDL_Init (SDL_INIT_EVERYTHING)) {
    goto FAILURE;
  }

  u = SDL_WINDOWPOS_CENTERED;
  v = FRACTAL_PIXEL;
  w = SDL_WINDOW_OPENGL;
  
  if (! (c->gfx.window = SDL_CreateWindow (FRACTAL_TITLE,u, u, v, v, w))) {
    goto FAILURE;
  }

  u = 0;
  v = SDL_RENDERER_ACCELERATED;
  
  if (! (c->gfx.render = SDL_CreateRenderer (c->gfx.window, u, v))) {
    goto FAILURE;
  }

  if (FRACTAL_FAILURE == c->shm.get (c)){
    goto FAILURE;
  }

  if (FRACTAL_FAILURE == c->shm.map (c)) {
    goto FAILURE;
  }

  u = FRACTAL_PIXEL;
  v = FRACTAL_DEPTH;
  w = FRACTAL_PIXEL * sizeof (w);
  j = 0xFF;
  k = 0xFF00;
  l = 0xFF0000;
  m = 0xFF000000;
  
  if (! (c->gfx.bitmap =
       SDL_CreateRGBSurfaceFrom (c->shm.b, u, u, v, w, j, k, l, m))) {
    goto FAILURE;
  }

  if (! (c->gfx.target =
	 SDL_CreateTextureFromSurface (c->gfx.render, c->gfx.bitmap))) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = EXIT_FAILURE;

 SUCCESS:

  return r;
}

#include <signal.h>

static u32 ctx_eve (ctx* c)
{
  u32 	    r = FRACTAL_SUCCESS;
  SDL_Event e ;

  while (SDL_PollEvent (&e)) {

    switch (e.type) {

    case SDL_QUIT: return FRACTAL_FAILURE;
      
    }
  }

  return r;
}

static u32 ctx_end (ctx* c)
{
  u32 r = FRACTAL_SUCCESS;

  if (! c) {
    goto FAILURE;
  }

  if (c->gfx.bitmap) {
    SDL_FreeSurface (c->gfx.bitmap);
    c->gfx.bitmap = 0;
  }

  if (c->gfx.target) {
    SDL_DestroyTexture (c->gfx.target);
    c->gfx.target = 0;
  }

  if (c->gfx.render) {
    SDL_DestroyRenderer (c->gfx.render);
    c->gfx.render = 0;
  }

  if (c->gfx.window) {
    SDL_DestroyWindow (c->gfx.window);
    c->gfx.window = 0;
  }
  
  if (0 < c->shm.d) {
    
    if (FRACTAL_FAILURE == c->shm.del (c)) {
      r = FRACTAL_FAILURE;
    }
    
  }

  if (0 < c->sem.d) {
    
      if (FRACTAL_FAILURE == c->sem.del (c)) {
	r = FRACTAL_FAILURE;
      }
      
  }

  SDL_Quit ();

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

/*************************************************************************
 *
 * FUN, DEP, LOC: KEYS
 * 
 *************************************************************************/

#include <sys/ipc.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: KEYS
 * 
 *************************************************************************/

static u32 key_get (nil* p, str f, s08 v)
{
  u32 	r = FRACTAL_SUCCESS;
  ctx*	c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > (c->key.d = ftok (f, v))) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

/*************************************************************************
 *
 * FUN, DEP, LOC: SEMAPHORE
 * 
 *************************************************************************/

#include <sys/sem.h>

#define SEMAPHORE_FLAG 0666 | IPC_CREAT

/*************************************************************************
 *
 * FUN, DEF, LOC: SEMAPHORE
 * 
 *************************************************************************/

static u32 sem_new (nil* p)
{
  u32 	r = FRACTAL_SUCCESS;
  ctx* 	c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > (c->sem.d = semget (c->key.d, c->sem.n, 0))) {
    if (0 > (c->sem.d = semget (c->key.d, c->sem.n, SEMAPHORE_FLAG))) {
      goto FAILURE;
    }
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 sem_del (nil* p)
{
  u32 	r = FRACTAL_SUCCESS;
  ctx* 	c = 0;

  if (! p) {
    goto FAILURE;
  }
  
  c = p;
  
  if (0 > c->sem.d) {
    goto FAILURE;
  }

  if (0 > semctl (c->sem.d, 1, IPC_RMID, 0)) {
    goto FAILURE;
  }
  
  c->sem.d = 0; goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 sem_inc (nil* p, u32 n)
{
  u32 		r = FRACTAL_SUCCESS;
  struct sembuf v = {0, 0, 0};
  ctx*		c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > c->sem.d) {
    goto FAILURE;
  }

  v.sem_num = n;
  v.sem_op	= 1;
  v.sem_flg	= 0;

  if (0 > semop (c->sem.d, &v, 1)) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 sem_dec (nil* p, u32 n)
{
  u32 		r = FRACTAL_SUCCESS;
  struct sembuf	v = {0, 0, 0};
  ctx*		c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > c->sem.d) {
    goto FAILURE;
  }

  v.sem_num =  n;
  v.sem_op  = -1;
  v.sem_flg =  0;

  if (0 > semop (c->sem.d, &v, 1)) {
    goto FAILURE;
  }

  goto SUCCESS;


 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 sem_set (nil* p, u32 n)
{
  u32 	r = FRACTAL_SUCCESS;
  ctx*	c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > c->sem.d) {
    goto FAILURE;
  }
  
  if (0 > semctl (c->sem.d, n, SETVAL, c->sem.v)) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 sem_get (nil* p, u32 n)
{
  u32  r = FRACTAL_SUCCESS;
  ctx* c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > (c->sem.d)) {
    goto FAILURE;
  }

  if (0 > (c->sem.v = semctl (c->sem.d, n, GETVAL, 0))) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

/*************************************************************************
 *
 * FUN, DEP, LOC: SHARED MEMORY
 * 
 *************************************************************************/

#include <sys/shm.h>

#define SHAREDMEM_FLAG 0666 | IPC_CREAT

/*************************************************************************
 *
 * FUN, DEF, LOC: SHARED MEMORY
 * 
 *************************************************************************/

static u32 shm_get (nil* p)
{
  u08  r = FRACTAL_SUCCESS;
  ctx* c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > (c->shm.d = shmget (c->key.d, FRACTAL_SPACE, 0))) {
    if (0 > (c->shm.d = shmget (c->key.d, FRACTAL_SPACE, SHAREDMEM_FLAG))) {
      goto FAILURE;
    }
  }
  
  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 shm_del (nil* p)
{
  u32  r = FRACTAL_SUCCESS;
  ctx* c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > shmctl (c->shm.d, IPC_RMID, 0)) {
    goto FAILURE;
  }

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 shm_map (nil* p)
{
  u32  r = FRACTAL_SUCCESS;
  ctx* c = 0;

  if (! p) {
    goto FAILURE;
  }
	
  c = p;

  if ((buf)0 > (c->shm.b = (shmat (c->shm.d, 0, 0)))) {
    goto FAILURE;
  }
  
  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;
}

static u32 shm_rem (nil* p)
{
  u32  r = FRACTAL_SUCCESS;
  ctx* c = 0;

  if (! p) {
    goto FAILURE;
  }

  c = p;

  if (0 > shmdt (c->shm.b)) {
    goto FAILURE;
  }
  
  goto SUCCESS;
  
 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  return r;

}

/*************************************************************************
 *
 * FUN, DEP, LOC: CALC
 * 
 *************************************************************************/

#include <math.h>

#ifdef PROGRAM_VECTOR_EXT
#include <immintrin.h>
#endif

/*************************************************************************
 *
 * FUN, DEF, LOC: CALC
 * 
 *************************************************************************/

#define CALC_ITERATIONS 0x40

static u32 calc (u32 Y, u32 X, u32 D)
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

static nil para (u32 Y, u32 X, u32 D, img A, u32 L)
{
  for (u32 i = 0; i < L; i++) {
    A [i] = calc (Y, X + i, D);
  }
}

/*************************************************************************
 *
 * FUN, DEP, LOC: WORK
 * 
 *************************************************************************/

#include <string.h>
#include <errno.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: WORK
 * 
 *************************************************************************/

static s32 work (ctx* c)
{
  s32 r = EXIT_SUCCESS;
  img w = 0;
  u32 u = 0;
  u32 x = FRACTAL_PIXEL / FRACTAL_PROCESS;
  u32 y = 0;
  u32 z = 0;

  if (! c) {
    goto FAILURE;
  }

  if ((! c->shm.d) || (0 > c->shm.d)) {
    goto FAILURE;
  }

  signal (SIGSEGV, jmp_sig);
  signal (SIGINT , jmp_sig);

  if (setjmp (f_jmp)) {
    goto FAILURE;
  }

  if (FRACTAL_FAILURE == c->shm.map (c)) {
    goto FAILURE;
  }
  
  for (;;) {
       
    if (sizeof (u) != read (c->pip.pro.channel.r, &u, sizeof (u))) {
      goto FAILURE;
    }

    if (u == 0xFFFF) {
      break;
    }

    y = u * x;
    z = y + x;
    w = (img)c->shm.b;
    
    for (u32 i = y, j = z; i < j; i++) {
#ifdef PROGRAM_VECTOR_EXT
      for (u32 j = 0; j < FRACTAL_PIXEL; j+=sizeof (u32)) {
	para (i, j, FRACTAL_PIXEL, w + i * FRACTAL_PIXEL + j, sizeof (u32));
#else
      for (u32 j = 0; j < FRACTAL_PIXEL; j++) {
	w [i * FRACTAL_PIXEL + j] = calc (i, j, FRACTAL_PIXEL);
#endif
      }
    }
    
    if (sizeof (u) != write (c->pip.con.channel.w, &u, sizeof (u))) {
      goto FAILURE;
    }

  }
  
  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  if (FRACTAL_FAILURE == c->shm.rem (c)) {
    r = FRACTAL_FAILURE;
  }

  return r;
}

/*************************************************************************
 *
 * DEP, DEF, LOC: WORK
 * 
 *************************************************************************/

#include <signal.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: WORK
 * 
 *************************************************************************/

static s32 loop (ctx* c)
{
  s32 r = FRACTAL_SUCCESS;
  u32 u = 0;
  u32 v = 0;
  s32 a = 0;
  s32 b = 0;

  if (! c) {
    goto FAILURE;
  }

  signal (SIGSEGV	, jmp_sig);
  signal (SIGINT	, jmp_sig);

  if (setjmp (f_jmp)) {

    u = 0xFFFF;

    for (u32 i = 0; i < FRACTAL_PROCESS; i++) {
          
      if (sizeof (u) != write (c->pip.pro.channel.w, &u, sizeof (u))) {
	goto FAILURE;
      }
    
    }
    
    goto SUCCESS;
  }

  if (FRACTAL_FAILURE == ctx_gfx (c)) {
    raise (SIGINT);
  }

  for (;;) {

    a = SDL_GetTicks ();

    if (FRACTAL_FAILURE == draw (c)) {
      break;
    }

    if (FRACTAL_FAILURE == ctx_eve (c)) {
      raise (SIGINT);
    }

    u = 0;
    v = 0;

    for (u32 i = 0; i < FRACTAL_PROCESS; i++) {
      
      if (sizeof (i) != write (c->pip.pro.channel.w, &i, sizeof (i))) {
	goto FAILURE;
      }

      u = u + i + 1;
    }   

    while (u) {
      
      if (sizeof (v) != read (c->pip.con.channel.r, &v, sizeof (v))) {
	goto FAILURE;
      }
      
      u = u - v - 1;

    }

    b = SDL_GetTicks () - a;

    if (b < FRACTAL_DELAY) {
      SDL_Delay (FRACTAL_DELAY - b);
    }

  }
	
  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:

  if (c) r = c->shm.rem (c);

  return r;
}

/*************************************************************************
 *
 * FUN, DEF, LOC: JMP
 * 
 *************************************************************************/

static nil jmp_sig (s32 sig)
{
  longjmp (f_jmp, 0x1234);
}

/*************************************************************************
 *
 * FUN, DEF, LOC: DRAW
 * 
 *************************************************************************/

static u32 draw (ctx* c)
{
  u32 		  r = FRACTAL_SUCCESS;
  static SDL_Rect a = {0, 0, FRACTAL_PIXEL, FRACTAL_PIXEL};
  static SDL_Rect b = {0, 0, FRACTAL_PIXEL, FRACTAL_PIXEL};

  if (! c) {
    goto FAILURE;
  }

  SDL_UpdateTexture (c->gfx.target, &a, c->shm.b, a.w * sizeof (u32));

  SDL_RenderClear (c->gfx.render);

  SDL_RenderCopy (c->gfx.render, c->gfx.target, &a, &b);

  SDL_RenderPresent (c->gfx.render);

  goto SUCCESS;

 FAILURE:

  r = FRACTAL_FAILURE;

 SUCCESS:
  
  return r;
}
